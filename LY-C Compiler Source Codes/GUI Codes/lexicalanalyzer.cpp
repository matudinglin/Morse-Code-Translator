#ifndef LEXICALANALYZER_H
#define LEXICALANALYZER_H

#include "erroranalyzer.h"
using namespace std;

struct wordPair
{
	string word;
	string type;
};

//词法分析器
class LexicalAnalyzer
{
	ErrorAnalyzer* ptrToErr;

	unordered_map<string, string> wordTable;
	vector<wordPair> result;
	vector<string> words;

public:
	//构造函数
	LexicalAnalyzer(ErrorAnalyzer* ptrToErr)
	{
		this->ptrToErr = ptrToErr;

		wordTable.insert({ "id","IDENFR" });
		wordTable.insert({ "cint","INTCON" });
		wordTable.insert({ "cchar","CHARCON" });
		wordTable.insert({ "string","STRCON" });
		wordTable.insert({ "const","CONSTTK" });
		wordTable.insert({ "int", "INTTK" });
		wordTable.insert({ "char", "CHARTK" });
		wordTable.insert({ "void", "VOIDTK" });
		wordTable.insert({ "main", "MAINTK" });
		wordTable.insert({ "if", "IFTK" });
		wordTable.insert({ "else", "ELSETK" });
		wordTable.insert({ "do", "DOTK" });
		wordTable.insert({ "while", "WHILETK" });
		wordTable.insert({ "for", "FORTK" });
		wordTable.insert({ "scanf", "SCANFTK" });
		wordTable.insert({ "printf", "PRINTFTK" });
		wordTable.insert({ "return", "RETURNTK" });
		wordTable.insert({ "+", "PLUS" });
		wordTable.insert({ "-", "MINU" });
		wordTable.insert({ "*", "MULT" });
		wordTable.insert({ "/", "DIV" });
		wordTable.insert({ "<", "LSS" });
		wordTable.insert({ "<=", "LEQ" });
		wordTable.insert({ ">", "GRE" });
		wordTable.insert({ ">=", "GEQ" });
		wordTable.insert({ "==", "EQL" });
		wordTable.insert({ "!=", "NEQ" });
		wordTable.insert({ "=", "ASSIGN" });
		wordTable.insert({ ";", "SEMICN" });
		wordTable.insert({ ",", "COMMA" });
		wordTable.insert({ "(", "LPARENT" });
		wordTable.insert({ ")", "RPARENT" });
		wordTable.insert({ "[", "LBRACK" });
		wordTable.insert({ "]", "RBRACK" });
		wordTable.insert({ "{", "LBRACE" });
		wordTable.insert({ "}", "RBRACE" });
	};


	//处理input
	void parse(string input)
	{
		separate(input);

		int wordSize = words.size();

		for (int i = 0; i < wordSize; i++)
		{	
			if (wordTable.find(words[i]) != wordTable.end())				//该单词不是前四类
			{
				result.push_back({ words[i],wordTable[words[i]] });
			}
			else
			{
				//int
				if (words[i][0] <= '9' && words[i][0] >= '0')
					result.push_back({ words[i],wordTable["cint"] });
				//char
				else if (words[i][0] == '\'')
				{
					words[i].erase(words[i].begin());
					words[i].erase(words[i].end() - 1);
					result.push_back({ words[i],wordTable["cchar"] });
				}
				//string
				else if (words[i][0] == '\"')
				{
					words[i].erase(words[i].begin());
					words[i].erase(words[i].end() - 1);
					result.push_back({ words[i],wordTable["string"] });
				}
				//id
				else if (isLetter(words[i][0]))
					result.push_back({ words[i],wordTable["id"] });
			}
		}
	};

	//处理成单词序列
	//处理成单词序列
	void separate(string input)
	{
		ptrToErr->lineNum.push_back(0);		//一行有多少个数
		int counter = 0;

		string temp;
		for (string::iterator i = input.begin(); i < input.end(); i++)
		{
			//前面无记录
			if (temp == "")
			{
				if (isDouble(*i) || isLetter(*i))
				{
					temp += *i;
				}
				else if (*i == '\'')
				{
					temp += *i;
					i++;
					while (*i != '\'')
					{
						temp += *i;
						i++;
					}
					temp += *i;
					words.push_back(temp);
					temp = "";
				}
				else if (*i == '\"')
				{
					temp += *i;
					i++;
					while (*i != '\"')
					{
						temp += *i;
						i++;
					}
					temp += *i;
					words.push_back(temp);
					temp = "";
				}
				else if (*i == '\n' || *i == ' ' || *i == '\t')
				{
					if (*i == '\n')
					{
						ptrToErr->lineNum.push_back(counter + ptrToErr->lineNum.back());
						counter = 0;
					}
				}
				else
				{
					words.insert(words.end(), temp += *i);
					counter++;
					temp = "";
				}
			}
			//前面有记录
			else
			{
				//><=!
				if (isDouble(temp[0]))
				{
					if (*i == '=')
					{
						temp += *i;
						words.insert(words.end(), temp);
						counter++;
						temp = "";
					}
					else
					{
						words.insert(words.end(), temp);
						counter++;
						temp = "";
						i--;
					}
				}
				else if (*i == '\n' || *i == ' ' || *i == '\t')
				{
					words.insert(words.end(), temp);
					counter++;
					temp = "";
					if (*i == '\n')
					{
						ptrToErr->lineNum.push_back(counter + ptrToErr->lineNum.back());
						counter = 0;
					}
				}
				else
				{
					if (isLetter(*i))
					{
						temp += *i;
					}
					else if (isDouble(*i))
					{
						words.insert(words.end(), temp);
						counter++;
						temp = "";
						temp += *i;
					}
					else
					{
						words.insert(words.end(), temp);
						counter++;
						temp = "";
						words.insert(words.end(), temp += *i);
						counter++;
						temp = "";
					}
				}
			}
		}
	};

	//返回结果
	string getResult()
	{
		string output;
		output += result[0].type + " " + result[0].word;
		for (unsigned i = 1; i < result.size(); i++)
			output += "\n" + result[i].type + " " + result[i].word;
		return output;
	};

	//是不是识别符
	bool isLetter(char ch)
	{
		if ((ch <= 'Z' && ch >= 'A') || (ch >= 'a' && ch <= 'z') || (ch <= '9' && ch >= '0') || ch == '_')
			return true;
		else
			return false;
	}

	//是不是双目运算符
	bool isDouble(char ch)
	{
		if (ch == '<' || ch == '>' || ch == '=' || ch == '!')
			return true;
		else
			return false;
	}

	// 复制result
	vector<wordPair> returnResult()
	{
		return result;
	}

	// 增加result项
	void appendResult(string word, string type)
	{
		result.push_back({ word, type });
	}

	// 判断是否为合法字符
	bool isLegalChar(char c)
	{
		bool flag = false;

		if ((c <= 'Z' && c >= 'A') || (c >= 'a' && c <= 'z') || (c <= '9' && c >= '0') || c == '_' || 
			c == '+' || c == '-' || c == '*' || c == '/')
			flag = true;

		return flag;
	}

	// 词法错误查找
	void searchError()
	{
		int resultSize = result.size();
		int lineSize = ptrToErr->lineNum.size();

		for (int i = 0, line = 0; i < resultSize; i++)
		{
			if (line < lineSize && i == ptrToErr->lineNum[line])
				line++;

			if (result[i].type == "CHARCON" && !isLegalChar(result[i].word[0]))
				ptrToErr->errorList.push_back({ line, "a" });
		}
	}
};

#endif
