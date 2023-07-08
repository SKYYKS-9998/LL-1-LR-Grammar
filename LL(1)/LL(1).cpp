#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
using namespace std;

//无左递归，默认为合法LL1

typedef vector<string> rightSide;
typedef map<string, rightSide> generations;
typedef map<string, vector<char>> FFTableItem;
typedef map<string, string> analyTableContent;
typedef map<char, analyTableContent> analyTableItem;

class LL1Grammar
{
public:
	LL1Grammar();
	~LL1Grammar();

	void inputGrammar(void);
	void toLL1(void);
	void analyInput(string input);

private:
	void formFirstTable(void);
	void getFirst(string n);
	void formFollowTable(void);
	void getFollow(void);
	void formAnalyTable(void);

	int isExistN(string s);
	int isExistT(char c);
	int isExistTinFirstTable(string left, char t);
	int isExistTinFollowTable(string left, char t);

	generations gens;
	vector<string> N;
	vector<char> T;
	FFTableItem firstTable;
	FFTableItem followTable;
	analyTableItem analyTable;
};

LL1Grammar::LL1Grammar()
{}

LL1Grammar::~LL1Grammar()
{}

void LL1Grammar::inputGrammar(void)
{
	int num = 0;

	//默认没有左递归
	cout << "Input number of non-terminators" << endl;
	cin >> num;
	cout << "Input non-terminators one by one" << endl;
	for (int i = num; i > 0; i--)
	{
		string tmp;
		cin >> tmp;
		if(!isExistN(tmp))
			this->N.push_back(tmp);
	}

	cout << "Input number of terminators" << endl;
	cin >> num;
	cout << "Input terminators one by one" << endl;
	for (int i = num; i > 0; i--)
	{
		char tmp;
		cin >> tmp;
		if (!isExistT(tmp))
			this->T.push_back(tmp);
	}

	cout << "Input number of generations" << endl;
	cin >> num;
	cout << "Input generations one by one" << endl;
	for (int i = num; i > 0; i--)
	{
		string left;
		string right;
		cout << "Input left part of the generation" << endl;
		cin >> left;
		cout << "Input right part of the generation" << endl;
		cin >> right;
		if (isExistN(left))
		{
			gens[left].push_back(right);
		}
	}
}

void LL1Grammar::toLL1(void)
{
	formFirstTable();
	formFollowTable();
	formAnalyTable();
}

void LL1Grammar::analyInput(string input)
{
	vector<string> stack;
	stack.push_back("$");
	stack.push_back(N[0]);
	input.push_back('$');
	string::iterator ip = input.begin();
	int step_count = 1;

	do 
	{
		if (isExistT((*stack.rbegin())[0]) || *stack.rbegin() == "$")
		{
			cout << "Step: " << step_count << endl;
			cout << "Stack: ";
			for (vector<string>::iterator it = stack.begin(); it != stack.end(); it++)
			{
				cout << *it;
			}
			cout << endl;
			cout << "Input: ";
			for (string::iterator it = ip; it != input.end(); it++)
				cout << *it;
			cout << endl;

			if ((*stack.rbegin())[0] == *ip)
			{
				if (*stack.rbegin() == "$")
					cout << "Output: Accept" << endl;
				cout << endl;

				stack.pop_back();
				ip++;
			}
			else
			{
				cout << "Output: Not accepted" << endl << endl;
				break;
			}
		}
		else
		{
			cout << "Step: " << step_count << endl;
			cout << "Stack: ";
			for (vector<string>::iterator it = stack.begin(); it != stack.end(); it++)
			{
				cout << *it;
			}
			cout << endl;
			cout << "Input: ";
			for (string::iterator it = ip; it != input.end(); it++)
				cout << *it;
			cout << endl;

			if (analyTable[*ip].count(*stack.rbegin()) != 0 && analyTable[*ip][*stack.rbegin()] != "synch")
			{
				cout << "Output: " << *stack.rbegin() << " -> " << analyTable[*ip][*stack.rbegin()] << endl;
				cout << endl;

				string tmp = *stack.rbegin();
				stack.pop_back();
				for (string::reverse_iterator rit = analyTable[*ip][tmp].rbegin(); rit != analyTable[*ip][tmp].rend(); rit++)
				{
					if (*rit == 'e')
						continue;

					if (*rit == '\'')
					{
						rit++;
						string tmp = string(1, *rit) + "\'";
						stack.push_back(tmp);
					}
					else
					{
						string tmp(1, *rit);
						stack.push_back(tmp);
					}
				}
			}
			else if(analyTable[*ip][*stack.rbegin()] == "synch")
			{
				cout << "Output: Error! M[" << *stack.rbegin() << ',' << *ip << "]=synch, skip " << *ip << endl << endl;
				ip++;
			}
			else
			{
				cout << "Output: Error! M[" << *stack.rbegin() << ',' << *ip << "] is empty, pop " << *stack.rbegin() << endl << endl;
				stack.pop_back();
			}
		}
		step_count++;
	} while (ip != input.end());

}

void LL1Grammar::formFirstTable(void)
{
	for (vector<string>::iterator iter = this->N.begin(); iter != this->N.end(); iter++)
		getFirst(*iter);
}

void LL1Grammar::getFirst(string n)
{
	int len = gens[n].size();
	for (int i = 0; i < len; i++)
	{
		if (isExistT(gens[n][i][0]))
		{
			if(!isExistTinFirstTable(n, gens[n][i][0]))
				firstTable[n].push_back(gens[n][i][0]);
		}
		else
		{
			string firstN;
			if (gens[n][i][1] == '\'')
				firstN = gens[n][i][0] + '\'';
			else
				firstN = gens[n][i][0];
			if (isExistN(firstN))
			{
				getFirst(firstN);
				for (vector<char>::iterator it = firstTable[firstN].begin(); it != firstTable[firstN].end(); it++)
					if (!isExistTinFirstTable(n, *it))
						firstTable[n].push_back(*it);
			}
		}
	}
}

void LL1Grammar::formFollowTable(void)
{
	vector<string>::iterator iter = this->N.begin();
	followTable[*iter].push_back('$');
	getFollow();
}

void LL1Grammar::getFollow(void)
{
	int change;
	do {
		change = 0;
		for (vector<string>::iterator nIt = N.begin(); nIt != N.end(); nIt++)
		{
			string n = *nIt;
			for (generations::iterator mapIt = gens.begin(); mapIt != gens.end(); mapIt++)
			{
				for (vector<string>::iterator genIt = (*mapIt).second.begin(); genIt != (*mapIt).second.end(); genIt++)
				{
					int strLoc = (*genIt).find(n);
					if (strLoc == string::npos)
						continue;
					while (n.size() == 1 && (*genIt)[strLoc + 1] == '\'')
					{
						strLoc = (*genIt).substr(strLoc + 1).find(n);
					}
					if (strLoc == string::npos)
						continue;
					if (n.size() == 2)
						strLoc++;

					if ((*genIt)[strLoc + 1] == '\0')
					{
						for (vector<char>::iterator followIt = followTable[(*mapIt).first].begin(); followIt != followTable[(*mapIt).first].end(); followIt++)
						{
							if (!isExistTinFollowTable(n, *followIt))
							{
								followTable[n].push_back(*followIt);
								change = 1;
							}
						}
					}
					else if (isExistT((*genIt)[strLoc + 1]))
					{
						if (!isExistTinFollowTable(n, (*genIt)[strLoc + 1]))
						{
							followTable[n].push_back((*genIt)[strLoc + 1]);
							change = 1;
						}
					}
					else
					{
						string followN;
						if ((*genIt)[strLoc + 2] == '\'')
						{
							followN = (*genIt)[strLoc + 1];
							followN += (*genIt)[strLoc + 2];
						}
						else
							followN = (*genIt)[strLoc + 1];

						if (isExistTinFirstTable(followN, 'e'))
						{
							for (vector<char>::iterator followIt = followTable[(*mapIt).first].begin(); followIt != followTable[(*mapIt).first].end(); followIt++)
							{
								if (!isExistTinFollowTable(n, *followIt))
								{
									followTable[n].push_back(*followIt);
									change = 1;
								}
							}
						}

						for (vector<char>::iterator firstIt = firstTable[followN].begin(); firstIt != firstTable[followN].end(); firstIt++)
						{
							if (!isExistTinFollowTable(n, *firstIt) && *firstIt != 'e')
							{
								followTable[n].push_back(*firstIt);
								change = 1;
							}
						}
					}
				}
			}
		}
	} while (change == 1);
}

void LL1Grammar::formAnalyTable(void)
{
	for (map<string, rightSide>::iterator gensIt = gens.begin(); gensIt != gens.end(); gensIt++)
	{
		for (rightSide::iterator singleGenIt = gensIt->second.begin(); singleGenIt != gensIt->second.end(); singleGenIt++)
		{
			if (isExistT((*singleGenIt)[0]))
			{
				analyTable[(*singleGenIt)[0]].emplace(gensIt->first, *singleGenIt);
				if ((*singleGenIt)[0] == 'e')
				{
					vector<char> followOfLeft = followTable[gensIt->first];

					for (vector<char>::iterator it = followOfLeft.begin(); it != followOfLeft.end(); it++)
					{
						analyTable[*it].emplace(gensIt->first, *singleGenIt);
					}
				}
			}
			else
			{
				string firstN;
				if ((*singleGenIt)[1] == '\'')
					firstN = string(1, (*singleGenIt)[0]) + "\'";
				else
					firstN = string(1, (*singleGenIt)[0]);

				int flagEmptyExist = 0;
				vector<char> firstOfN = firstTable[firstN];

				for (vector<char>::iterator it = firstOfN.begin(); it != firstOfN.end(); it++)
				{
					if (*it == 'e')
					{
						flagEmptyExist = 1;
						continue;
					}
					analyTable[*it].emplace(gensIt->first, *singleGenIt);
				}

				if (flagEmptyExist)
				{
					vector<char> followOfLeft = followTable[gensIt->first];

					for (vector<char>::iterator it = followOfLeft.begin(); it != followOfLeft.end(); it++)
					{
						analyTable[*it].emplace(gensIt->first, *singleGenIt);
					}
				}
			}
		}
	}

	for (vector<string>::iterator itN = N.begin(); itN != N.end(); itN++)
	{
		for (vector<char>::iterator itFollow = followTable[*itN].begin(); itFollow != followTable[*itN].end(); itFollow++)
		{
			if (analyTable[*itFollow].count(*itN) == 0)
				analyTable[*itFollow][*itN] = "synch";
		}
	}
}

int LL1Grammar::isExistN(string s)
{
	if (find(N.begin(), N.end(), s) != N.end())
		return 1;
	else
		return 0;
}

int LL1Grammar::isExistT(char c)
{
	if (find(T.begin(), T.end(), c) != T.end())
		return 1;
	else
		return 0;
}

int LL1Grammar::isExistTinFirstTable(string left, char t)
{
	if (find(firstTable[left].begin(), firstTable[left].end(), t) != firstTable[left].end())
		return 1;
	else
		return 0;
}

int LL1Grammar::isExistTinFollowTable(string left, char t)
{
	if (find(followTable[left].begin(), followTable[left].end(), t) != followTable[left].end())
		return 1;
	else
		return 0;
}

int main(void)
{
	string userinput, input;
	LL1Grammar g;
	g.inputGrammar();
	g.toLL1();

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
				while (it != userinput.end()&& *it >= '0' && *it <= '9')
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

		g.analyInput(input);
	}

	system("pause");
	return 0;
}