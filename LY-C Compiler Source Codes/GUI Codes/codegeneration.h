#ifndef CODEGENERATION_H
#define CODEGENERATION_H

#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <vector>
using namespace std;

struct QuaFormula
{
	string lineNum;
	string optor;
	string operand1;
	string operand2;
	string dest;
};

class CodeGenerator
{
	vector<QuaFormula> formulaList;

public:
	int tempVarCounter;
	int lineStart;

	//vector<int> ifHeadReg;
	vector<vector<int>> LHReg;
	vector<bool> isClosedFlag;

	CodeGenerator();
	void addFormula(QuaFormula);
	string getFormula();

	void addLHReg();
	void deleteLHReg();

	void addClosedFlag();
	void deleteClosedFlag();

    int tempVarDis(bool isInc = true);
    void addIfJump(int offset = 0);
	void addWhileJump();
	void addDoWhileJump();
	void addForJump();
	void addDirectJump();
};



#endif //CODEGENERATION_H
