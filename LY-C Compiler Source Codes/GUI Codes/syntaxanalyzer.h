#ifndef SYNTAXANALYZER_H
#define SYNTAXANALYZER_H


#include"lexicalanalyzer.h"
#include"semanticanalyzer.h"
using namespace std;

struct syntaxPair
{
	int index;
	string name;
};

class SyntaxAnalyzer
{
	ErrorAnalyzer* ptrToErr;
	LexicalAnalyzer* ptrToLex;
	SemanticAnalyzer* ptrToSem;
	CodeGenerator* ptrToCod;

	vector<wordPair> inputs;
	vector<syntaxPair> results;
	vector<errorPair> synErrorReg;

	int inputSize;
	int processLine;

public:

	SyntaxAnalyzer(ErrorAnalyzer*, LexicalAnalyzer*, SemanticAnalyzer*, CodeGenerator*);
	bool isAddOper(int&);			// 加法运算符
	bool isMulOper(int&);			// 乘法运算符
	bool isRelOper(int&);			// 关系运算符
	bool isLetter(int&);			// 字母
	bool isDigit(int&);				// 数字
	bool isNZeroDigit(int&);		// 非零数字
	bool isChar(int&);				// 字符
	bool isString(int&);			// 字符串
	bool isProgram(int&);			// 程序
	bool isConstDec(int&);			// 常量说明
	bool isConstDef(int&);			// 常量定义
	bool isUnsignedInt(int&);		// 无符号整数
	bool isInt(int&);				// 整数
	bool isInd(int&);				// 标识符
	bool isDecHead(int&);			// 声明头部
	bool isVarDec(int&);			// 变量说明
	bool isVarDef(int&);			// 变量定义
	bool isTypeInd(int&);			// 类型标识符
	bool isFuncRVDef(int&);			// 有返回值函数定义
	bool isFuncNRVDef(int&);		// 无返回值函数定义
	bool isComState(int&);			// 复合语句
	bool isParaList(int&);			// 参数表
	bool isMainFunc(int&);			// 主函数
	bool isExpression(int&);		// 表达式
	bool isTerm(int&);				// 项
	bool isFact(int&);				// 因子
	bool isStatement(int&);			// 语句
	bool isAssignState(int&);		// 赋值语句
	bool isCondState(int&);			// 条件语句
	bool isCondition(int&);			// 条件
	bool isLoopState(int&);			// 循环语句
	bool isStepSize(int&);			// 步长
	bool isFuncRVCall(int&);		// 有返回值函数调用语句
	bool isFuncNRVCall(int&);		// 无返回值函数调用语句
	bool isVParaList(int&);			// 值参数表
	bool isStateColumn(int&);		// 语句列
	bool isReadState(int&);			// 读语句
	bool isWriteState(int&);		// 写语句
	bool isReturnState(int&);		// 返回语句

	bool isSame(int&, string);

	string getOutputs();			// 以字符串形式返回结果
	void sendError();				// 将成员synErrorReg（词法分析错误寄存器）拷贝到errorAnalyzer的errorList（错误表）中
	void callFunc(int, int);		// 函数调用

	void addLoopFlag();
	void deleteLoopFlag();
	void addCallFuncFlag();
	void deleteCallFuncFlag();

    void clear()
    {
        inputs.clear();
        results.clear();
        synErrorReg.clear();
    }
};



#endif
