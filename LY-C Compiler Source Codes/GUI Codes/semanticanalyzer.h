#ifndef SEMANTICANALYZER_H
#define SEMANTICANALYZER_H


#include"erroranalyzer.h"
#include"codegeneration.h"
#include<typeinfo>
using namespace std;

struct SymbolElem
{
	string listName;
	string name;
	int index;
};

struct IntElem
{
	string type;	// 常量 / 变量
	string name;
	int value;
};

struct CharElem
{
	string type;
	string name;
	char value;
};

struct IntArray
{
	string name;
	int* data;
	int size;
};

struct CharArray
{
	string name;
	char* data;
	int size;
};

struct FuncRegElem
{
	string defRVType;
	string actRVType;
	string name;
	bool hasRV;
	vector<SymbolElem> paraList;
	int paraCounter;
	int startPointer;
	int fHead;
};

struct FuncReg
{
	bool flag;
	int rPointer;
	FuncRegElem reg;
};

struct FuncElem
{
	string RVType;
	string name;
	vector<SymbolElem> paraList;
	int startPointer;
	int fHead;
};

struct ifRegElem
{
	bool condStateFlag;
	bool conditionFlag;
	bool ifStatus;
	int ifReg;
};

struct returnElem
{
	bool flag;
	string dataType;
	int intReg;
	char charReg;
};

struct expRegElem
{
	bool flag;
	string dataType;
	vector<IntElem> intExpReg;
	vector<CharElem> charExpReg;
};

class SemanticAnalyzer
{
	ErrorAnalyzer* ptrToErr;
	CodeGenerator* ptrToCod;
	
	vector<IntElem> intList;
	vector<CharElem> charList;
	vector<IntArray> intArrList;
	vector<CharArray> charArrList;
	vector<FuncElem> funcList;

	vector<errorPair> semErrorReg;

	int intListSize;
	int charListSize;
	int intArrListSize;
	int charArrListSize;
	int funcSize;
	int symTableSize;

public:
	int synInputPtr;
	vector<SymbolElem> SymTable;

	FuncReg defFuncReg;
	vector<FuncReg> callFuncReg;

	bool mainFuncFlag;
	int mainFuncPointer;

	vector<expRegElem> expReg;

	returnElem returnReg;

	vector<ifRegElem> conditionReg;
	string ifNameReg;

	vector<bool> LoopFlagSSS;
	bool VParaListFlag;
	vector<bool> CallFuncFlagSSS;

	SemanticAnalyzer(ErrorAnalyzer*, CodeGenerator*);


	void addIntElem(int, string, string, int);
	void addCharElem(int, string, string, char);
	void addArray(int, string, string, int);
	void addFunc();

	void addIntReg(string, string, int);
	void addCharReg(string, string, char);
	void addRegInd(int);
	void regOperate(string);
	void assignToElem(int);
	void arrValueToExpReg(int);
	string getTopRegType();
	int getTopIntRegValue();
	string outputRegTop();
	void outputString(string);
	void inputVar(string);
	void addExpReg();
	void deleteExpReg();

	void addCondReg();
	void deleteCondReg();
	void setIfFlag(string);
	void resetIfReg();

	void sendError();

	string getElemType(string);
	int getSymSize();
	int getSymIndex(string);

	void addSemError(errorPair);
	void resetDefFuncReg();
	void addCallFuncReg();
	void deleteCallFuncReg();
	void addParaElem(string, string);

	int getFuncIndex(int);
	FuncElem getFunc(int);
	bool callFuncCheck(int, int);

	string arrayNamePro(string, int);
};




#endif
