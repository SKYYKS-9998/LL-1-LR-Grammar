#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;
#define SHIFT 0
#define REDUCE 1
#define ACCEPT 2

typedef struct
{
	char left;
	string right;
}generation;
typedef struct 
{
	int type;
	int no;
}actionType;
typedef map<char, actionType> actionContent;
typedef vector<actionContent> actionTable;
typedef map<char, int> gotoContent;
typedef vector<gotoContent> gotoTable;

class LRanalyTable
{
public:
	LRanalyTable();
	~LRanalyTable();

	void inputTable();
	void analysis(string input);

private:
	void output(int step, vector<int> stateStack, vector<char> symbolStack, string& input, string::iterator& ip);
	int isN(char n);
	int isT(char t);

	vector<char> N;
	vector<char> T;
	vector<generation> genForms;
	actionTable actionT;
	gotoTable gotoT;
};

LRanalyTable::LRanalyTable()
{}

LRanalyTable::~LRanalyTable()
{}

void LRanalyTable::inputTable()
{
	int num = 0;

	cout << "Input number of non-terminators" << endl;
	cin >> num;
	cout << "Input non-terminators one by one" << endl;
	for (int i = num; i > 0; i--)
	{
		char tmp;
		cin >> tmp;
		this->N.push_back(tmp);
	}

	cout << "Input number of terminators" << endl;
	cin >> num;
	cout << "Input terminators one by one" << endl;
	for (int i = num; i > 0; i--)
	{
		char tmp;
		cin >> tmp;
		this->T.push_back(tmp);
	}
	this->T.push_back('$');

	cout << "Input number of generations" << endl;
	cin >> num;
	cout << "Input generations one by one" << endl;
	for (int i = num; i > 0; i--)
	{
		char left;
		string right;
		cout << "Input left part of the generation" << endl;
		cin >> left;
		cout << "Input right part of the generation" << endl;
		cin >> right;
		genForms.push_back(generation({left, right}));
	}

	cout << "Input number of states" << endl;
	cin >> num;
	actionT = actionTable(num);
	gotoT = gotoTable(num);
	for (int i = 0; i < num; i++)
	{
		for (int j = 0; j < T.size(); j++)
		{
			int type, no;
			cout << "Input type of action[S" << i << ", " << T[j] << "]" << endl;
			cout << "0 for shift, 1 for reduce by, 2 for accept, 3 for empty" << endl;
			cin >> type;
			if (type == 3)
				continue;
			if (type != ACCEPT)
			{
				cout << "Input action[S" << i << ", " << T[j] << "]" << endl;
				cin >> no;
				actionType tmp = { type, no };
				actionT[i][T[j]] = tmp;
			}
			else
			{
				actionType tmp = { type, 0 };
				actionT[i][T[j]] = tmp;
			}
		}
	}

	for (int i = 0; i < num; i++)
	{
		for (int j = 1; j < N.size(); j++)
		{
			int no;
			cout << "Input goto[S" << i << ", " << N[j] << "]" << endl;
			cout << "Input -1 for empty item" << endl;
			cin >> no;
			if (no == -1)
				continue;
			gotoT[i][N[j]] = no;
		}
	}
}

void LRanalyTable::analysis(string input)
{
	vector<int> stateStack;
	vector<char> symbolStack;
	int stepCount = 1;

	stateStack.push_back(0);
	input.push_back('$');
	string::iterator ip = input.begin();

	do
	{
		if (actionT[stateStack.back()].count(*ip) != 0)
		{
			if (actionT[stateStack.back()][*ip].type == SHIFT)
			{
				output(stepCount, stateStack, symbolStack, input, ip);

				symbolStack.push_back(*ip);
				stateStack.push_back(actionT[stateStack.back()][*ip].no);
				ip++;
			}
			else if (actionT[stateStack.back()][*ip].type == REDUCE)
			{
				output(stepCount, stateStack, symbolStack, input, ip);

				int rightLen = genForms[actionT[stateStack.back()][*ip].no].right.size();
				char newTop = genForms[actionT[stateStack.back()][*ip].no].left;
				for (int i = 0; i < rightLen; i++)
				{
					stateStack.pop_back();
					symbolStack.pop_back();
				}
				symbolStack.push_back(newTop);
				stateStack.push_back(gotoT[stateStack.back()][newTop]);
			}
			else if (actionT[stateStack.back()][*ip].type == ACCEPT)
			{
				output(stepCount, stateStack, symbolStack, input, ip);
				return;
			}
		}
		else 
		{
			cout << "Step: " << stepCount << endl;
			cout << "State Stack: ";
			for (vector<int>::iterator it = stateStack.begin(); it != stateStack.end(); it++)
			{
				cout << *it;
			}
			cout << endl;
			cout << "Symbol Stack: ";
			for (vector<char>::iterator it = symbolStack.begin(); it != symbolStack.end(); it++)
			{
				cout << *it;
			}
			cout << endl;
			cout << "Input: ";
			for (string::iterator it = ip; it != input.end(); it++)
			{
				cout << *it;
			}
			cout << endl;
			cout << "Output: Not Accept" << endl << endl;
			break;
		}
		stepCount++;
	} while (1);
}

void LRanalyTable::output(int step, vector<int> stateStack, vector<char> symbolStack, string& input, string::iterator& ip)
{
	cout << "Step: " << step << endl;
	cout << "State Stack: ";
	for (vector<int>::iterator it = stateStack.begin(); it != stateStack.end(); it++)
	{
		cout << *it;
	}
	cout << endl;
	cout << "Symbol Stack: ";
	for (vector<char>::iterator it = symbolStack.begin(); it != symbolStack.end(); it++)
	{
		cout << *it;
	}
	cout << endl;
	cout << "Input: ";
	for (string::iterator it = ip; it != input.end(); it++)
	{
		cout << *it;
	}
	cout << endl;
	switch (actionT[stateStack.back()][*ip].type)
	{
	case SHIFT:
		cout << "Output: Shift ";
		cout << actionT[stateStack.back()][*ip].no << endl << endl;
		break;
	case REDUCE:
		cout << "Output: Reduce ";
		cout << genForms[actionT[stateStack.back()][*ip].no].left << "->" << genForms[actionT[stateStack.back()][*ip].no].right << endl << endl;
		break;
	case ACCEPT:
		cout << "Output: Accept" << endl << endl;
		break;
	default:
		break;
	}
}

int LRanalyTable::isN(char n)
{
	if (find(N.begin(), N.end(), n) != N.end())
		return 1;
	else
		return 0;
}

int LRanalyTable::isT(char t)
{
	if (find(T.begin(), T.end(), t) != T.end())
		return 1;
	else
		return 0;
}

int main(void)
{
	string userinput, input;
	LRanalyTable lrTable;
	lrTable.inputTable();

	while (1)
	{
		cout << "请输入要分析的符号串，输入quit退出" << endl;
		cin >> userinput;
		input.clear();

		if (userinput == "quit")
			break;

		int state = 0;
		for (string::iterator it = userinput.begin(); it != userinput.end(); it++)
		{
			switch (state)
			{
			case 0:
				if (*it >= '0' && *it <= '9')
				{
					input.push_back('n');
					state = 1;
				}
				else
					input.push_back(*it);
				break;
			case 1:
				while (it != userinput.end() && *it >= '0' && *it <= '9')
				{
					it++;
				}
				state = 0;
				if (it != userinput.end())
					input.push_back(*it);
				else
					it--;
				break;
			default:
				break;
			}
		}

		lrTable.analysis(input);
	}

	system("pause");
	return 0;
}