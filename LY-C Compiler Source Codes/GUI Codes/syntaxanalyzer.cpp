#include "syntaxanalyzer.h"


SyntaxAnalyzer::SyntaxAnalyzer(ErrorAnalyzer* ptrToErr, LexicalAnalyzer* ptrToLex, SemanticAnalyzer* ptrToSem, CodeGenerator* ptrToCod)
{
    this->ptrToErr = ptrToErr;
    this->ptrToLex = ptrToLex;
    this->ptrToSem = ptrToSem;
    this->ptrToCod = ptrToCod;


}


bool SyntaxAnalyzer::isSame(int& pointer, string s)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    if (inputs[pt++].type == s)
    {
        flag = true;
        pointer = pt;
    }

    return flag;
}

bool SyntaxAnalyzer::isAddOper(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    if (isSame(pt, "PLUS") || isSame(pt, "MINU"))
    {
        flag = true;
        pointer = pt;
    }

    return flag;
}

bool SyntaxAnalyzer::isMulOper(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    if (isSame(pt, "MULT") || isSame(pt, "DIV"))
    {
        flag = true;
        pointer = pt;
    }

    return flag;
}

bool SyntaxAnalyzer::isRelOper(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    string s = inputs[pt].type;

    if (s == "LSS" || s == "LEQ" || s == "GRE" || s == "GEQ" || s == "EQL" || s == "NEQ")
    {
        flag = true;
        pointer++;
    }

    return flag;
}

bool SyntaxAnalyzer::isLetter(int&)
{
    return true;
}

bool SyntaxAnalyzer::isDigit(int&)
{
    return true;
}

bool SyntaxAnalyzer::isNZeroDigit(int&)
{
    return true;
}

bool SyntaxAnalyzer::isChar(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    if (isSame(pt, "CHARCON"))
    {
        flag = true;
        pointer = pt;
    }

    return flag;
}

bool SyntaxAnalyzer::isString(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    if (isSame(pt, "STRCON"))
    {
        flag = true;
        pointer = pt;
    }

    if (flag && ptrToSem->LoopFlagSSS.empty() && ptrToSem->CallFuncFlagSSS.empty())
        results.push_back({ pointer, "<字符串>" });

    return flag;
}

bool SyntaxAnalyzer::isProgram(int& pointer)
{
    this->inputs = ptrToLex->returnResult();
    this->inputSize = this->inputs.size();
    this->processLine = 0;

    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    isConstDec(pt);
    isVarDec(pt);

    while (isFuncRVDef(pt) || isFuncNRVDef(pt));

    if (isMainFunc(pt))
    {
        flag = true;
        pointer = pt;
    }

    if (flag && ptrToSem->LoopFlagSSS.empty() && ptrToSem->CallFuncFlagSSS.empty())
        results.push_back({ pointer, "<程序>" });

    return flag;
}

bool SyntaxAnalyzer::isConstDec(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    if (isSame(pt, "CONSTTK") && isConstDef(pt) && isSame(pt, "SEMICN"))
    {
        flag = true;
        pointer = pt;

        while (isSame(pt, "CONSTTK") && isConstDef(pt) && isSame(pt, "SEMICN"))
            pointer = pt;
    }

    if (flag && ptrToSem->LoopFlagSSS.empty() && ptrToSem->CallFuncFlagSSS.empty())
        results.push_back({ pointer, "<常量说明>" });

    return flag;
}

bool SyntaxAnalyzer::isConstDef(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    if (isSame(pt, "INTTK") && isInd(pt) && isSame(pt, "ASSIGN"))
    {
        int ptt = pt;

        if (isInt(pt))
        {
            flag = true;
            pointer = pt;

            int ptt1 = ptt;
            if (isAddOper(ptt1))
                ptrToSem->addIntElem(ptt - 2, "const", inputs[ptt - 2].word, (int)atoi((inputs[ptt].word + inputs[ptt + 1].word).c_str()));			// 语义分析：将常量定义加入符号表
            else
                ptrToSem->addIntElem(ptt - 2, "const", inputs[ptt - 2].word, (int)atoi(inputs[ptt].word.c_str()));

            while (isSame(pt, "COMMA") && isInd(pt) && isSame(pt, "ASSIGN"))
            {
                int ptt = pt;

                if (isInt(pt))
                {
                    pointer = pt;

                    int ptt1 = ptt;
                    if (isAddOper(ptt1))
                        ptrToSem->addIntElem(ptt - 2, "const", inputs[ptt - 2].word, (int)atoi((inputs[ptt].word + inputs[ptt + 1].word).c_str()));			// 语义分析：将常量定义加入符号表
                    else
                        ptrToSem->addIntElem(ptt - 2, "const", inputs[ptt - 2].word, (int)atoi(inputs[ptt].word.c_str()));
                }
            }
        }

        else if (isString(pt))			//错误o：常量定义中=后面只能是整型或字符型常量
        {
            synErrorReg.push_back({ ptt, "o" });

            flag = true;
            pointer = pt;
        }
    }

    else
    {
        pt = pointer;		// 回到初始位置检查是否为char型

        if (isSame(pt, "CHARTK") && isInd(pt) && isSame(pt, "ASSIGN"))
        {
            int ptt = pt;

            if (isChar(pt))
            {
                flag = true;
                pointer = pt;

                ptrToSem->addCharElem(pointer - 3, "const", inputs[pointer - 3].word, (char)inputs[pointer - 1].word[0]);			// 语义分析：将常量定义加入符号表

                while (isSame(pt, "COMMA") && isInd(pt) && isSame(pt, "ASSIGN") && isChar(pt))
                {
                    pointer = pt;

                    ptrToSem->addCharElem(pointer - 3, "const", inputs[pointer - 3].word, (char)inputs[pointer - 1].word[0]);		// 语义分析：将常量定义加入符号表
                }
            }

            else if (isString(pt))			//错误o：常量定义中=后面只能是整型或字符型常量
            {
                synErrorReg.push_back({ ptt, "o" });

                flag = true;
                pointer = pt;
            }
        }
    }

    if (flag && ptrToSem->LoopFlagSSS.empty() && ptrToSem->CallFuncFlagSSS.empty())
        results.push_back({ pointer, "<常量定义>" });

    return flag;
}

bool SyntaxAnalyzer::isUnsignedInt(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    if (isSame(pt, "INTCON"))
    {
        flag = true;
        pointer = pt;
    }

    if (flag && ptrToSem->LoopFlagSSS.empty() && ptrToSem->CallFuncFlagSSS.empty())
        results.push_back({ pointer, "<无符号整数>" });

    return flag;
}

bool SyntaxAnalyzer::isInt(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    string optor = "none";

    if (isAddOper(pt))
        if (inputs[pt - 1].word == "-")
            optor = "neg";

    if (isUnsignedInt(pt))
    {
        flag = true;
        pointer = pt;

        if (!ptrToSem->expReg.empty() && ptrToSem->expReg.back().flag)
        {
            ptrToSem->addIntReg("inst", inputs[pointer - 1].word, (int)atoi(inputs[pointer - 1].word.c_str()));
            ptrToSem->regOperate(optor);
        }
    }

    if (flag && ptrToSem->LoopFlagSSS.empty() && ptrToSem->CallFuncFlagSSS.empty())
        results.push_back({ pointer, "<整数>" });

    return flag;
}

bool SyntaxAnalyzer::isInd(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    if (isSame(pt, "IDENFR"))
    {
        flag = true;
        pointer = pt;

        if (ptrToSem->defFuncReg.flag && ptrToSem->defFuncReg.reg.name == "")
            ptrToSem->defFuncReg.reg.name = inputs[pointer - 1].word;
        else if (!ptrToSem->callFuncReg.empty() && ptrToSem->callFuncReg.back().flag && ptrToSem->callFuncReg.back().reg.name == "")
            ptrToSem->callFuncReg.back().reg.name = inputs[pointer - 1].word;
    }

    return flag;
}

bool SyntaxAnalyzer::isDecHead(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    if (isSame(pt, "INTTK") && isInd(pt))
    {
        flag = true;
        pointer = pt;
    }

    else
    {
        pt = pointer;

        if (isSame(pt, "CHARTK") && isInd(pt))
        {
            flag = true;
            pointer = pt;
        }
    }

    if (flag && ptrToSem->LoopFlagSSS.empty() && ptrToSem->CallFuncFlagSSS.empty())
        results.push_back({ pointer, "<声明头部>" });

    return flag;
}

bool SyntaxAnalyzer::isVarDec(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    if (isVarDef(pt) && isSame(pt, "SEMICN"))
    {
        flag = true;
        pointer = pt;

        if (ptrToSem->LoopFlagSSS.empty() && ptrToSem->CallFuncFlagSSS.empty())
            results.push_back({ pointer - 1, "<变量定义>" });

        while (isVarDef(pt) && isSame(pt, "SEMICN"))
        {
            pointer = pt;

            if (ptrToSem->LoopFlagSSS.empty() && ptrToSem->CallFuncFlagSSS.empty())
                results.push_back({ pointer - 1, "<变量定义>" });
        }
    }

    if (flag && ptrToSem->LoopFlagSSS.empty() && ptrToSem->CallFuncFlagSSS.empty())
        results.push_back({ pointer, "<变量说明>" });

    return flag;
}

bool SyntaxAnalyzer::isVarDef(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    int ptt = pt + 2;

    if (isTypeInd(pt) && isInd(pt) && !isSame(ptt, "LPARENT"))
    {
        flag = true;
        pointer = pt;

        string dataType = inputs[pointer - 2].word;

        if (isSame(pt, "LBRACK") && isUnsignedInt(pt))
        {
            if (isSame(pt, "RBRACK"))
            {
                pointer = pt;

                ptrToSem->addArray(pointer - 4, dataType,inputs[pointer - 4].word, (int)atoi(inputs[pointer - 2].word.c_str()));
            }

            else							// 错误m：应为右中括号’]’
            {
                synErrorReg.push_back({ pt - 1, "m" });

                pointer = pt;
            }
        }

        else
        {
            pt = pointer;

            if (dataType == "int")
                ptrToSem->addIntElem(pointer - 1, "var", inputs[pointer - 1].word, 0);
            else if (dataType == "char")
                ptrToSem->addCharElem(pointer - 1, "var", inputs[pointer - 1].word, ' ');
        }


        while (isSame(pt, "COMMA") && isInd(pt))
        {
            pointer = pt;

            if (isSame(pt, "LBRACK") && isUnsignedInt(pt))
            {
                if (isSame(pt, "RBRACK"))
                {
                    pointer = pt;

                    ptrToSem->addArray(pointer - 4, dataType, inputs[pointer - 4].word, (int)atoi(inputs[pointer - 2].word.c_str()));
                }

                else							// 错误m：应为右中括号’]’
                {
                    synErrorReg.push_back({ pt - 1, "m" });

                    pointer = pt;
                }
            }

            else
            {
                if (dataType == "int")
                    ptrToSem->addIntElem(pointer - 1, "var", inputs[pointer - 1].word, 0);
                else if (inputs[pointer - 2].word == "char")
                    ptrToSem->addCharElem(pointer - 1, "var", inputs[pointer - 1].word, ' ');
            }
        }
    }

    return flag;
}

bool SyntaxAnalyzer::isTypeInd(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    if (isSame(pt, "INTTK") || isSame(pt, "CHARTK"))
    {
        flag = true;
        pointer = pt;
    }

    return flag;
}

bool SyntaxAnalyzer::isFuncRVDef(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    ptrToSem->defFuncReg.flag = true;
    if (!ptrToSem->callFuncReg.empty() && ptrToSem->callFuncReg.back().flag)
        ptrToSem->defFuncReg.flag = false;

    ptrToSem->defFuncReg.reg.startPointer = ptrToSem->getSymSize();
    ptrToSem->defFuncReg.reg.fHead = pt;
    int RVHead = pt;

    if (isDecHead(pt) && isSame(pt, "LPARENT") && isParaList(pt) && isSame(pt, "RPARENT") &&
        isSame(pt, "LBRACE") && isComState(pt) && isSame(pt, "RBRACE"))
    {
        flag = true;
        pointer = pt;

        ptrToSem->defFuncReg.reg.defRVType = inputs[RVHead].word;
        ptrToSem->addFunc();
    }

    ptrToSem->defFuncReg.flag = false;
    ptrToSem->resetDefFuncReg();

    if (flag && ptrToSem->LoopFlagSSS.empty() && ptrToSem->CallFuncFlagSSS.empty())
        results.push_back({ pointer, "<有返回值函数定义>" });

    return flag;
}

bool SyntaxAnalyzer::isFuncNRVDef(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    ptrToSem->defFuncReg.flag = true;
    ptrToSem->defFuncReg.reg.startPointer = ptrToSem->getSymSize();
    ptrToSem->defFuncReg.reg.fHead = pt;

    if (isSame(pt, "VOIDTK") && isInd(pt) && isSame(pt, "LPARENT") && isParaList(pt) &&
        isSame(pt, "RPARENT") && isSame(pt, "LBRACE") && isComState(pt) && isSame(pt, "RBRACE"))
    {
        flag = true;
        pointer = pt;

        ptrToSem->defFuncReg.reg.defRVType = "void";
        ptrToSem->addFunc();
    }

    ptrToSem->defFuncReg.flag = false;
    ptrToSem->resetDefFuncReg();

    if (flag && ptrToSem->LoopFlagSSS.empty() && ptrToSem->CallFuncFlagSSS.empty())
        results.push_back({ pointer, "<无返回值函数定义>" });

    return flag;
}

bool SyntaxAnalyzer::isComState(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    if (isConstDec(pt))
        pointer = pt;
    if (isVarDec(pt))
        pointer = pt;

    if (isStateColumn(pt))
    {
        flag = true;
        pointer = pt;
    }

    if (flag && ptrToSem->LoopFlagSSS.empty() && ptrToSem->CallFuncFlagSSS.empty())
        results.push_back({ pointer, "<复合语句>" });

    return flag;
}

bool SyntaxAnalyzer::isParaList(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    flag = true;
    if (isTypeInd(pt) && isInd(pt))
    {
        pointer = pt;

        string type = inputs[pointer - 2].word;
        if (type == "int")
        {
            ptrToSem->addParaElem("int", inputs[pointer - 1].word);
            ptrToSem->addIntElem(pointer - 1, "var", inputs[pointer - 1].word, 0);
        }
        else if (type == "char")
        {
            ptrToSem->addParaElem("char", inputs[pointer - 1].word);
            ptrToSem->addCharElem(pointer - 1, "var", inputs[pointer - 1].word, ' ');
        }

        while (isSame(pt, "COMMA") && isTypeInd(pt) && isInd(pt))
        {
            pointer = pt;

            string type = inputs[pointer - 2].word;
            if (type == "int")
            {
                ptrToSem->addParaElem("int", inputs[pointer - 1].word);
                ptrToSem->addIntElem(pointer - 1, "var", inputs[pointer - 1].word, 0);
            }
            else if (type == "char")
            {
                ptrToSem->addParaElem("char", inputs[pointer - 1].word);
                ptrToSem->addCharElem(pointer - 1, "var", inputs[pointer - 1].word, ' ');
            }
        }
    }

    if (flag && ptrToSem->LoopFlagSSS.empty() && ptrToSem->CallFuncFlagSSS.empty())
        results.push_back({ pointer, "<参数表>" });

    return flag;
}

bool SyntaxAnalyzer::isMainFunc(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    ptrToSem->mainFuncFlag = true;
    ptrToSem->mainFuncPointer = ptrToSem->getSymSize();

    if (isSame(pt, "VOIDTK") && isSame(pt, "MAINTK") && isSame(pt, "LPARENT") && isSame(pt, "RPARENT") &&
        isSame(pt, "LBRACE") && isComState(pt) && isSame(pt, "RBRACE"))
    {
        flag = true;
        pointer = pt;
    }

    if (flag && ptrToSem->LoopFlagSSS.empty() && ptrToSem->CallFuncFlagSSS.empty())
        results.push_back({ pointer, "<主函数>" });

    return flag;
}

bool SyntaxAnalyzer::isExpression(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    string optor = "none";

    if (isAddOper(pt))
    {
        pointer = pt;

        if (inputs[pt - 1].word == "-")
            optor = "neg";
    }

    if (isTerm(pt))
    {
        flag = true;
        pointer = pt;

        int opHead = pt;
        while (isAddOper(pt) && isTerm(pt))
        {
            pointer = pt;

            if (!ptrToSem->expReg.empty() && ptrToSem->expReg.back().flag)
            {
                ptrToSem->regOperate(inputs[opHead].word);
                opHead = pt;
            }
        }
    }

    if (flag && ptrToSem->LoopFlagSSS.empty() && ptrToSem->CallFuncFlagSSS.empty())
        results.push_back({ pointer, "<表达式>" });

    return flag;
}

bool SyntaxAnalyzer::isTerm(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    if (isFact(pt))
    {
        flag = true;
        pointer = pt;

        int opHead = pt;
        while (isMulOper(pt) && isFact(pt))
        {
            pointer = pt;

            if (!ptrToSem->expReg.empty() && ptrToSem->expReg.back().flag)
            {
                ptrToSem->regOperate(inputs[opHead].word);
                opHead = pt;
            }
        }
    }

    if (flag && ptrToSem->LoopFlagSSS.empty() && ptrToSem->CallFuncFlagSSS.empty())
        results.push_back({ pointer, "<项>" });

    return flag;
}

bool SyntaxAnalyzer::isFact(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    int pHead = pt;

    if (isInt(pt) || isChar(pt))
    {
        flag = true;
        pointer = pt;

        if (!ptrToSem->expReg.empty() && ptrToSem->expReg.back().flag && inputs[pt - 1].type == "CHARCON")
            ptrToSem->addCharReg("inst", "\'" + inputs[pt - 1].word + "\'", (char)inputs[pt - 1].word[0]);
    }

    else if (isFuncRVCall(pt))
    {
        flag = true;
        pointer = pt;
    }

    else if (isInd(pt))
    {
        flag = true;
        pointer = pt;

        int index = ptrToSem->getSymIndex(inputs[pt - 1].word);
        bool isValid = true;

        if (index == ptrToSem->getSymSize())			// 错误c：未定义的名字
        {
            ptrToSem->addSemError({ pt - 1, "c" });
            isValid = false;
        }

        if (isValid && !ptrToSem->expReg.empty() && ptrToSem->expReg.back().flag)
                ptrToSem->addRegInd(index);


        //数组
        ptrToSem->addExpReg();
        if (isSame(pt, "LBRACK") && isExpression(pt) && isSame(pt, "RBRACK"))
        {
            pointer = pt;

            bool condFlag = !ptrToSem->conditionReg.empty() && ptrToSem->conditionReg.back().condStateFlag &&
                !ptrToSem->conditionReg.back().conditionFlag && !ptrToSem->conditionReg.back().ifStatus && !ptrToCod->isClosedFlag.empty() && ptrToCod->isClosedFlag.back();

            if (isValid && !ptrToSem->expReg.empty() && !condFlag)			// 错误i：数组元素的下标只能是整型表达式
            {
                if (ptrToSem->expReg.back().dataType != "int")
                {
                    ptrToSem->addSemError({ pHead, "i" });
                    isValid = false;
                }
                else
                    ptrToSem->regOperate("[]");
            }

            ptrToSem->deleteExpReg();

            if (isValid && !ptrToSem->expReg.empty() && ptrToSem->expReg.back().flag && !condFlag)
                ptrToSem->arrValueToExpReg(index);
        }

        else
            ptrToSem->deleteExpReg();
    }

    else if (isSame(pt, "LPARENT") && isExpression(pt) && isSame(pt, "RPARENT"))
    {
        flag = true;
        pointer = pt;
    }

    if (flag && ptrToSem->LoopFlagSSS.empty() && ptrToSem->CallFuncFlagSSS.empty())
        results.push_back({ pointer, "<因子>" });

    return flag;
}

bool SyntaxAnalyzer::isStatement(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    if (isCondState(pt) || isLoopState(pt))
    {
        flag = true;
        pointer = pt;
    }

    else if (isSame(pt, "LBRACE"))
    {
        if (isStateColumn(pt) && isSame(pt, "RBRACE"))
        {
            flag = true;
            pointer = pt;
        }

        else
            pt = pointer;
    }

    else if (isFuncNRVCall(pt) || isFuncRVCall(pt) || isAssignState(pt) || isReadState(pt) || isWriteState(pt) || isReturnState(pt))
    {
        if (isSame(pt, "SEMICN"))
        {
            flag = true;
            pointer = pt;
        }

        else				// 错误k：应为分号（缺分号）
        {
            synErrorReg.push_back({ pt - 1, "k" });

            flag = true;
            pointer = pt;
        }
    }

    else if (isSame(pt, "SEMICN"))
    {
        flag = true;
        pointer = pt;
    }

    if (flag && ptrToSem->LoopFlagSSS.empty() && ptrToSem->CallFuncFlagSSS.empty())
        results.push_back({ pointer, "<语句>" });

    return flag;
}

bool SyntaxAnalyzer::isAssignState(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    int ptt = pt;

    ptrToSem->addExpReg();

    if (isInd(pt))
    {
        int index = ptrToSem->getSymIndex(inputs[ptt].word);

        if (isSame(pt, "ASSIGN"))
        {
            ptrToSem->addRegInd(index);

            if (isExpression(pt))
            {
                flag = true;
                pointer = pt;

                string type = ptrToSem->getElemType(inputs[ptt].word);
                if (type == "const")						// 错误j：不能改变常量的值
                    ptrToSem->addSemError({ ptt, "j" });
                else if (type == "none")					// 错误c：未定义的名字
                    ptrToSem->addSemError({ ptt, "c" });

                else
                {
                    ptrToSem->regOperate("=");
                    ptrToSem->assignToElem(index);
                }
            }

            else
                pt = pointer;
        }

        else if (isSame(pt, "LBRACK"))
        {
            ptrToSem->addRegInd(index);

            ptrToSem->addExpReg();
            if (isExpression(pt))
            {
                int isValid = false;
                bool condFlag = !ptrToSem->conditionReg.empty() && ptrToSem->conditionReg.back().condStateFlag &&
                    !ptrToSem->conditionReg.back().conditionFlag && !ptrToSem->conditionReg.back().ifStatus && !ptrToCod->isClosedFlag.empty() && ptrToCod->isClosedFlag.back();

                string type = ptrToSem->getElemType(inputs[ptt].word);
                if (type == "none")																	// 错误c：未定义的名字
                    ptrToSem->addSemError({ ptt, "c" });

                if (!condFlag)
                {
                    if (!ptrToSem->expReg.empty() && ptrToSem->expReg.back().dataType != "int")			// 错误i：数组元素的下标只能是整型表达式
                        ptrToSem->addSemError({ ptt, "i" });
                    else
                    {
                        ptrToSem->regOperate("[]");
                        isValid = true;
                    }
                }

                ptrToSem->deleteExpReg();


                if (isSame(pt, "RBRACK") && isSame(pt, "ASSIGN") && isExpression(pt))
                {
                    flag = true;
                    pointer = pt;

                    if (isValid && !condFlag)
                    {
                        ptrToSem->regOperate("=");
                        ptrToSem->synInputPtr = ptt;
                        ptrToSem->assignToElem(index);
                    }
                }
            }

            else
                ptrToSem->deleteExpReg();
        }
    }

    ptrToSem->deleteExpReg();

    if (flag && ptrToSem->LoopFlagSSS.empty() && ptrToSem->CallFuncFlagSSS.empty())
        results.push_back({ pointer, "<赋值语句>" });

    return flag;
}

bool SyntaxAnalyzer::isCondState(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    ptrToSem->addCondReg();
    ptrToCod->addLHReg();

    if (isSame(pt, "IFTK") && isSame(pt, "LPARENT") && isCondition(pt) &&
        isSame(pt, "RPARENT") && isStatement(pt))
    {
        flag = true;
        pointer = pt;

        ptrToSem->conditionReg.back().ifStatus = !ptrToSem->conditionReg.back().ifStatus;

        if (isSame(pt, "ELSETK"))
        {
            ptrToCod->addIfJump(1);
            ptrToCod->addDirectJump();

            if (isStatement(pt))
            {
                pointer = pt;

                ptrToCod->addIfJump();
            }
        }
        else
            ptrToCod->addIfJump();
    }

    ptrToCod->deleteLHReg();
    ptrToSem->deleteCondReg();

    if (flag && ptrToSem->LoopFlagSSS.empty() && ptrToSem->CallFuncFlagSSS.empty())
        results.push_back({ pointer, "<条件语句>" });

    return flag;
}

bool SyntaxAnalyzer::isCondition(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    ptrToSem->addExpReg();
    ptrToSem->conditionReg.back().conditionFlag = true;

    int ptt = pt;

    if (isExpression(pt))
    {
        flag = true;
        pointer = pt;

        if (ptrToSem->getTopRegType() != "int")			// 错误f：条件判断中出现不合法的类型
            ptrToSem->addSemError({ ptt, "f" });
        ptt = pt;

        if (ptrToSem->getTopRegType() == "int")			// 设置if语义
        {
            ptrToSem->conditionReg.back().ifReg = ptrToSem->getTopIntRegValue();
            ptrToSem->setIfFlag("");
        }

        ptrToSem->ifNameReg = ptrToSem->expReg.back().intExpReg.back().name;

        ptrToSem->deleteExpReg();
        ptrToSem->addExpReg();

        int RelOpHead = pt;
        if (isRelOper(pt) && isExpression(pt))
        {
            pointer = pt;

            if (ptrToSem->getTopRegType() != "int")			// 错误f：条件判断中出现不合法的类型
                ptrToSem->addSemError({ ptt + 1, "f" });
            ptt = pt;

            if (ptrToSem->getTopRegType() == "int")			// 设置if语义
                ptrToSem->setIfFlag(inputs[RelOpHead].word);
        }
        else
            ptrToSem->setIfFlag("@");
    }

    ptrToSem->conditionReg.back().conditionFlag = false;
    ptrToSem->deleteExpReg();

    if (flag && ptrToSem->LoopFlagSSS.empty() && ptrToSem->CallFuncFlagSSS.empty())
        results.push_back({ pointer, "<条件>" });

    return flag;
}

bool SyntaxAnalyzer::isLoopState(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    ptrToSem->addCondReg();
    ptrToCod->addLHReg();

    // While语句
    if (isSame(pt, "WHILETK"))
    {
        if (isSame(pt, "LPARENT"))
        {
            int loopConHead = pt;

            if (!ptrToCod->LHReg.empty())
                ptrToCod->LHReg.back().push_back(ptrToCod->lineStart);

            if (isCondition(pt) && isSame(pt, "RPARENT"))
            {
                int stateHead = pt;

                if (isStatement(pt))
                {
                    flag = true;
                    pointer = pt;

                    ptrToCod->addFormula({ to_string(ptrToCod->lineStart), "j", "_", "_", "100" });
                    ptrToCod->addIfJump();
                    ptrToCod->addWhileJump();

                    addLoopFlag();
                    ptrToCod->addClosedFlag();
                    while (!ptrToSem->defFuncReg.flag && !ptrToSem->conditionReg.empty() && ptrToSem->conditionReg.back().ifStatus)
                    {
                        int tempLoopHead = loopConHead;
                        int tempStateHead = stateHead;

                        isStatement(tempStateHead);
                        isCondition(tempLoopHead);
                    }
                    ptrToCod->deleteClosedFlag();
                    deleteLoopFlag();
                }
            }
        }

        else
            pt = pointer;
    }

    // Do-While语句
    else if (isSame(pt, "DOTK"))
    {
        int stateHead = pt;
        ptrToSem->conditionReg.back().ifStatus= true;

        if (!ptrToCod->LHReg.empty())
            ptrToCod->LHReg.back().push_back(ptrToCod->lineStart);

        if (isStatement(pt))
        {
            int ptt = pt;

            if (isSame(pt, "WHILETK") && isSame(pt, "LPARENT"))
            {
                int loopConHead = pt;

                if (isCondition(pt) && isSame(pt, "RPARENT"))
                {
                    flag = true;
                    pointer = pt;

                    ptrToCod->addDoWhileJump();

                    addLoopFlag();
                    ptrToCod->addClosedFlag();
                    while (!ptrToSem->defFuncReg.flag && !ptrToSem->conditionReg.empty() && ptrToSem->conditionReg.back().ifStatus)
                    {
                        int tempStateHead = stateHead;
                        int tempLoopHead = loopConHead;

                        isStatement(tempStateHead);
                        isCondition(tempLoopHead);
                    }
                    ptrToCod->deleteClosedFlag();
                    deleteLoopFlag();
                }
            }

            else
            {
                pt = ptt;

                if (isSame(pt, "LPARENT") && isCondition(pt) && isSame(pt, "RPARENT"))
                {
                    this->synErrorReg.push_back({ ptt, "n" });						// 错误n：do-while语句中缺少while

                    pointer = pt;
                }
            }
        }

        else pt = pointer;
    }

    // For语句
    else if (isSame(pt, "FORTK"))
    {
        if (isSame(pt, "LPARENT"))
        {
            int nameHead = pt;

            if (isInd(pt))
            {
                int index = ptrToSem->getSymIndex(inputs[nameHead].word);
                ptrToSem->addExpReg();
                ptrToSem->conditionReg.back().ifStatus = true;
                ptrToSem->addRegInd(index);

                if (isSame(pt, "ASSIGN") && isExpression(pt) && isSame(pt, "SEMICN"))
                {
                    ptrToSem->regOperate("=");
                    ptrToSem->assignToElem(index);
                    ptrToSem->deleteExpReg();

                    if (!ptrToCod->LHReg.empty())
                        ptrToCod->LHReg.back().push_back(ptrToCod->lineStart);

                    int loopConHead = pt;
                    if (isCondition(pt) && isSame(pt, "SEMICN"))
                    {
                        int reNameHead1 = pt;

                        if (isInd(pt) && isSame(pt, "ASSIGN"))
                        {
                            int reNameHead2 = pt;

                            if (isInd(pt))
                            {
                                int reIndex1 = ptrToSem->getSymIndex(inputs[reNameHead1].word);
                                int reIndex2 = ptrToSem->getSymIndex(inputs[reNameHead2].word);

                                int opHead = pt;
                                if (isAddOper(pt))
                                {
                                    int stepHead = pt;

                                    if (isStepSize(pt) && isSame(pt, "RPARENT"))
                                    {
                                        int stateHead = pt;
                                        if (isStatement(pt))
                                        {
                                            flag = true;
                                            pointer = pt;

                                            if (ptrToSem->conditionReg.back().ifStatus || ptrToCod->isClosedFlag.empty() || !ptrToCod->isClosedFlag.back())
                                            {
                                                ptrToSem->addExpReg();
                                                ptrToSem->addRegInd(reIndex1);
                                                ptrToSem->addRegInd(reIndex2);
                                                ptrToSem->addIntReg("inst", inputs[stepHead].word, (int)atoi(inputs[stepHead].word.c_str()));
                                                ptrToSem->regOperate(inputs[opHead].word);
                                                ptrToSem->regOperate("=");
                                                if (ptrToSem->conditionReg.back().ifStatus)
                                                    ptrToSem->assignToElem(reIndex1);
                                                ptrToSem->deleteExpReg();
                                            }

                                            ptrToCod->addForJump();

                                            addLoopFlag();
                                            ptrToCod->addClosedFlag();
                                            int tempLoopHead = loopConHead;
                                            isCondition(tempLoopHead);

                                            while ((!ptrToSem->defFuncReg.flag || (!ptrToSem->callFuncReg.empty() && ptrToSem->callFuncReg.back().flag)) && !ptrToSem->conditionReg.empty() && ptrToSem->conditionReg.back().ifStatus)
                                            {
                                                int tempLoopHead = loopConHead;
                                                int tempStateHead = stateHead;

                                                isStatement(tempStateHead);

                                                ptrToSem->addExpReg();
                                                ptrToSem->addRegInd(reIndex2);
                                                ptrToSem->addIntReg("inst", inputs[stepHead].word, (int)atoi(inputs[stepHead].word.c_str()));
                                                ptrToSem->regOperate(inputs[opHead].word);
                                                ptrToSem->assignToElem(reIndex1);
                                                ptrToSem->deleteExpReg();

                                                isCondition(tempLoopHead);
                                            }
                                            ptrToCod->deleteClosedFlag();
                                            deleteLoopFlag();
                                        }
                                    }
                                }
                            }
                        }
                    }
                }

                ptrToSem->deleteExpReg();
            }
        }
    }

    ptrToCod->deleteLHReg();
    ptrToSem->deleteCondReg();

    if (flag && ptrToSem->LoopFlagSSS.empty() && ptrToSem->CallFuncFlagSSS.empty())
        results.push_back({ pointer, "<循环语句>" });

    return flag;
}

bool SyntaxAnalyzer::isStepSize(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    if (isUnsignedInt(pt))
    {
        flag = true;
        pointer = pt;
    }

    if (flag && ptrToSem->LoopFlagSSS.empty() && ptrToSem->CallFuncFlagSSS.empty())
        results.push_back({ pointer, "<步长>" });

    return flag;
}

bool SyntaxAnalyzer::isFuncRVCall(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    int fHeadp = pointer;
    ptrToSem->addCallFuncReg();

    if (isInd(pt) && isSame(pt, "LPARENT") && isVParaList(pt) && isSame(pt, "RPARENT"))
    {
        flag = true;
        pointer = pt;

        if (ptrToSem->mainFuncFlag)
        {
            int index = ptrToSem->getFuncIndex(0);
            ptrToSem->callFuncReg.back().reg.hasRV = true;
            callFunc(fHeadp, index);
        }

        ptrToCod->addFormula({ to_string(ptrToCod->lineStart), "CALL", "_", "_", ptrToSem->callFuncReg.back().reg.name });
    }

    ptrToSem->deleteCallFuncReg();

    if (flag && ptrToSem->LoopFlagSSS.empty() && ptrToSem->CallFuncFlagSSS.empty())
        results.push_back({ pointer, "<有返回值函数调用语句>" });

    return flag;
}

bool SyntaxAnalyzer::isFuncNRVCall(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    int fHeadp = pointer;
    ptrToSem->addCallFuncReg();

    if (isInd(pt) && isSame(pt, "LPARENT") && isVParaList(pt) && isSame(pt, "RPARENT"))
    {
        flag = true;
        pointer = pt;

        if (ptrToSem->mainFuncFlag)
        {
            int index = ptrToSem->getFuncIndex(0);
            ptrToSem->callFuncReg.back().reg.hasRV = false;
            callFunc(fHeadp, index);
        }

        ptrToCod->addFormula({ to_string(ptrToCod->lineStart), "CALL", "_", "_", ptrToSem->callFuncReg.back().reg.name });
    }

    ptrToSem->deleteCallFuncReg();

    if (flag && ptrToSem->LoopFlagSSS.empty() && ptrToSem->CallFuncFlagSSS.empty())
        results.push_back({ pointer, "<无返回值函数调用语句>" });

    return flag;
}

bool SyntaxAnalyzer::isVParaList(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    flag = true;

    ptrToSem->addExpReg();
    ptrToSem->VParaListFlag = true;

    if (isExpression(pt))
    {
        int paraIndex = 0;

        int funcParaSize = 0;
        vector<SymbolElem> paraList;
        if (ptrToSem->mainFuncFlag)
        {
            FuncElem func = ptrToSem->getFunc(ptrToSem->getFuncIndex(0));
            funcParaSize = (int)func.paraList.size();
            paraList = func.paraList;
        }
        else
        {
            funcParaSize = (int)ptrToSem->defFuncReg.reg.paraList.size();
            paraList = ptrToSem->defFuncReg.reg.paraList;
        }


        pointer = pt;
        if (!ptrToSem->callFuncReg.empty() && ptrToSem->callFuncReg.back().flag)
        {
            string type = ptrToSem->getTopRegType();
            ptrToSem->callFuncReg.back().reg.paraList.push_back({ type, "none", -1 });
            if (paraIndex < funcParaSize)
                ptrToSem->assignToElem(paraList[paraIndex++].index);

            ptrToSem->callFuncReg.back().reg.paraCounter++;
        }

        ptrToSem->deleteExpReg();
        ptrToSem->addExpReg();

        while (isSame(pt, "COMMA") && isExpression(pt))
        {
            pointer = pt;
            if (!ptrToSem->callFuncReg.empty() && ptrToSem->callFuncReg.back().flag)
            {
                string type = ptrToSem->getTopRegType();
                ptrToSem->callFuncReg.back().reg.paraList.push_back({ type, "none", -1 });
                if (paraIndex < funcParaSize)
                    ptrToSem->assignToElem(paraList[paraIndex++].index);

                ptrToSem->callFuncReg.back().reg.paraCounter++;
            }

            ptrToSem->deleteExpReg();
            ptrToSem->addExpReg();
        }
    }

    ptrToSem->VParaListFlag = false;
    ptrToSem->deleteExpReg();

    if (flag && ptrToSem->LoopFlagSSS.empty() && ptrToSem->CallFuncFlagSSS.empty())
        results.push_back({ pointer, "<值参数表>" });

    return flag;
}

bool SyntaxAnalyzer::isStateColumn(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    flag = true;

    while (isStatement(pt))
        pointer = pt;

    if (flag && ptrToSem->LoopFlagSSS.empty() && ptrToSem->CallFuncFlagSSS.empty())
        results.push_back({ pointer, "<语句列>" });

    return flag;
}

bool SyntaxAnalyzer::isReadState(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    if (isSame(pt, "SCANFTK") && isSame(pt, "LPARENT") && isInd(pt))
    {
        int ptT = pt;

        string name = inputs[pt - 1].word;
        ptrToSem->inputVar(name);
        ptrToCod->addFormula({ to_string(ptrToCod->lineStart), "IN", "_", "_", name });

        while (isSame(ptT, "COMMA") && isInd(ptT))
        {
            pt = ptT;

            name = inputs[pt - 1].word;
            ptrToSem->inputVar(name);
            ptrToCod->addFormula({ to_string(ptrToCod->lineStart), "IN", "_", "_", name });
        }


        if (isSame(pt, "RPARENT"))
        {
            flag = true;
            pointer = pt;
        }
    }

    if (flag && ptrToSem->LoopFlagSSS.empty() && ptrToSem->CallFuncFlagSSS.empty())
        results.push_back({ pointer, "<读语句>" });

    return flag;
}

bool SyntaxAnalyzer::isWriteState(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    ptrToSem->addExpReg();

    if (isSame(pt, "PRINTFTK") && isSame(pt, "LPARENT"))
    {
        int strHead = pt;

        if (isString(pt))
        {
            int ptt = pt;

            //ptrToCod->addFormula({ to_string(ptrToCod->lineStart), "=", inputs[strHead].word, "_", "t" + to_string(ptrToCod->tempVarCounter++) });

            ptrToSem->outputString(inputs[pt - 1].word);

            if (isSame(pt, "RPARENT"))
            {
                flag = true;
                pointer = pt;

                ptrToCod->addFormula({ to_string(ptrToCod->lineStart), "OUT","\"" + inputs[strHead].word + "\"", "_", "_" });
            }

            else if (isSame(ptt, "SEMICN"))				// 错误l：应为右小括号’)’
            {
                synErrorReg.push_back({ pt, "l" });

                flag = true;
                pointer = pt;
            }

            else if (isSame(pt, "COMMA") && isExpression(pt))
            {
                int ptt1 = pt;

                string outName = ptrToSem->outputRegTop();
                ptrToCod->addFormula({ to_string(ptrToCod->lineStart), "OUT", "\"" + inputs[strHead].word + "\"", outName, "_" });

                if (isSame(pt, "RPARENT"))
                {
                    flag = true;
                    pointer = pt;
                }

                else if (isSame(ptt1, "SEMICN"))		// 错误l：应为右小括号’)’
                {
                    synErrorReg.push_back({ pt, "l" });

                    flag = true;
                    pointer = pt;
                }
            }

            else
                pt = pointer;
        }

        else if (isExpression(pt))
        {
            int ptt = pt;

            string outName = ptrToSem->outputRegTop();
            ptrToCod->addFormula({ to_string(ptrToCod->lineStart), "OUT", outName,"_", "_" });

            if (isSame(pt, "RPARENT"))
            {
                flag = true;
                pointer = pt;
            }

            else if (isSame(ptt, "SEMICN"))				// 错误l：应为右小括号’)’
            {
                synErrorReg.push_back({ pt, "l" });

                flag = true;
                pointer = pt;
            }
        }
    }

    if (flag)
        ptrToSem->outputString("\n");

    ptrToSem->deleteExpReg();

    if (flag && ptrToSem->LoopFlagSSS.empty() && ptrToSem->CallFuncFlagSSS.empty())
        results.push_back({ pointer, "<写语句>" });

    return flag;
}

bool SyntaxAnalyzer::isReturnState(int& pointer)
{
    if (pointer >= inputSize)
        return false;

    bool flag = false;
    int pt = pointer;

    int headPointer = pointer;

    ptrToSem->addExpReg();

    if (isSame(pt, "RETURNTK"))
    {
        flag = true;
        pointer = pt;

        if (isSame(pt, "LPARENT") && isExpression(pt) && isSame(pt, "RPARENT"))
        {
            pointer = pt;

            if (ptrToSem->defFuncReg.flag)
                ptrToSem->defFuncReg.reg.hasRV = true;

            if ((int)ptrToSem->expReg.size() >= 2)
            {
                if (ptrToSem->expReg.back().dataType == "int" && !ptrToSem->expReg.back().intExpReg.empty())
                    (ptrToSem->expReg.end() - 2)->intExpReg.push_back(ptrToSem->expReg.back().intExpReg.back());
                else if (ptrToSem->expReg.back().dataType == "char" && !ptrToSem->expReg.back().charExpReg.empty())
                    (ptrToSem->expReg.end() - 2)->charExpReg.push_back(ptrToSem->expReg.back().charExpReg.back());
            }
        }
    }

    if (flag && ptrToSem->defFuncReg.flag && !ptrToSem->expReg.empty())
    {
        ptrToSem->defFuncReg.reg.actRVType = ptrToSem->expReg.back().dataType;
        ptrToSem->defFuncReg.rPointer = headPointer;
    }

    ptrToSem->deleteExpReg();

    if (flag && ptrToSem->LoopFlagSSS.empty() && ptrToSem->CallFuncFlagSSS.empty())
        results.push_back({ pointer, "<返回语句>" });

    return flag;
}


string SyntaxAnalyzer::getOutputs()
{
    string output;
    unsigned int pairTop = 0;

    output += inputs[0].type + " " + inputs[0].word;
    for (int i = 1; i < inputSize; i++)
    {
        while (pairTop < results.size() && results[pairTop].index == i)
            output += "\n" + results[pairTop++].name;

        output += "\n" + inputs[i].type + " " + inputs[i].word;
    }

    while (pairTop < results.size() && results[pairTop].index == inputSize)
        output += "\n" + results[pairTop++].name;

    return output;
}

void SyntaxAnalyzer::sendError()
{
    int synErrorSize = this->synErrorReg.size();
    int lineSize = ptrToErr->lineNum.size();

    for (int i = 0, line = 0; i < synErrorSize; i++)
    {
        while (line < lineSize && synErrorReg[i].lineInd >= ptrToErr->lineNum[line])
            line++;

        ptrToErr->errorList.push_back({ line, synErrorReg[i].errorType });
    }
}

void SyntaxAnalyzer::callFunc(int fHeadp, int index)
{
    bool isValid = ptrToSem->callFuncCheck(fHeadp, index);
    addCallFuncFlag();

    // 函数调用执行
    if (ptrToSem->mainFuncFlag && isValid)
    {
        FuncElem func = ptrToSem->getFunc(ptrToSem->getFuncIndex(0));
        int tempHead = func.fHead;

        ptrToCod->addClosedFlag();
        if (ptrToSem->callFuncReg.back().reg.hasRV)
            isFuncRVDef(tempHead);
        else
            isFuncNRVDef(tempHead);
        ptrToCod->deleteClosedFlag();
    }

    deleteCallFuncFlag();
}

void SyntaxAnalyzer::addLoopFlag()
{
    ptrToSem->LoopFlagSSS.push_back(true);
}

void SyntaxAnalyzer::deleteLoopFlag()
{
    if (!ptrToSem->LoopFlagSSS.empty())
        ptrToSem->LoopFlagSSS.pop_back();
}

void SyntaxAnalyzer::addCallFuncFlag()
{
    ptrToSem->CallFuncFlagSSS.push_back(true);
}

void SyntaxAnalyzer::deleteCallFuncFlag()
{
    if (!ptrToSem->CallFuncFlagSSS.empty())
        ptrToSem->CallFuncFlagSSS.pop_back();
}
