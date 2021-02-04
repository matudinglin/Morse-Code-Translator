#include "semanticanalyzer.h"
#include <string.h>

SemanticAnalyzer::SemanticAnalyzer(ErrorAnalyzer* ptrToErr, CodeGenerator* ptrToCod)
{
    this->ptrToErr = ptrToErr;
    this->ptrToCod = ptrToCod;

    this->intListSize = 0;
    this->charListSize = 0;
    this->intArrListSize = 0;
    this->charArrListSize = 0;
    this->funcSize = 0;
    this->symTableSize = 0;

    this->defFuncReg.flag = false;
    this->resetDefFuncReg();

    this->mainFuncFlag = false;
    this->mainFuncPointer = 0;

    this->returnReg.flag = false;
    this->returnReg.dataType = "void";

    this->VParaListFlag = false;
}


void SemanticAnalyzer::addIntElem(int pointer, string type, string name, int value)
{
    if (!callFuncReg.empty() && callFuncReg.back().flag)
        return;

    bool isValid = true;

    int start = defFuncReg.reg.startPointer;
    if (mainFuncFlag == true)
        start = mainFuncPointer;


    for (int i = start; isValid && i < symTableSize; i++)
        if (SymTable[i].name == name)			// 错误b：名字重定义
        {
            semErrorReg.push_back({ pointer, "b" });
            isValid = false;
            break;
        }

    if (isValid)
    {
        intList.push_back({ type, name, value });
        SymTable.push_back({ "int", name, intListSize });

        intListSize++;
        symTableSize++;
    }
}

void SemanticAnalyzer::addCharElem(int pointer, string type, string name, char value)
{
    if (!callFuncReg.empty() && callFuncReg.back().flag)
        return;

    bool isValid = true;

    int start = defFuncReg.reg.startPointer;
    if (mainFuncFlag == true)
        start = mainFuncPointer;

    for (int i = start; isValid && i < symTableSize; i++)
        if (SymTable[i].name == name)			// 错误b：名字重定义
        {
            semErrorReg.push_back({ pointer, "b" });
            isValid = false;
            break;
        }

    if (isValid)
    {
        charList.push_back({ type, name, value });
        SymTable.push_back({ "char", name, charListSize });

        charListSize++;
        symTableSize++;
    }
}

void SemanticAnalyzer::addArray(int pointer, string dataType, string name, int size)
{
    if (!callFuncReg.empty() && callFuncReg.back().flag)
        return;

    bool isValid = true;

    int start = defFuncReg.reg.startPointer;
    if (mainFuncFlag == true)
        start = mainFuncPointer;

    for (int i = start; isValid && i < symTableSize; i++)
        if (SymTable[i].name == name)			// 错误b：名字重定义
        {
            semErrorReg.push_back({ pointer, "b" });
            isValid = false;
            break;
        }

    if (isValid)
    {
        if (dataType == "int")
        {
            int* data = new int[size];
            memset(data, 0, size * sizeof(int));
            intArrList.push_back({ name, data, size });
            SymTable.push_back({ "intArray", name, intArrListSize });

            intArrListSize++;
            symTableSize++;
        }

        else if (dataType == "char")
        {
            char* data = new char[size];
            memset(data, ' ', size * sizeof(char));
            charArrList.push_back({ name, data, size });
            SymTable.push_back({ "charArray", name, charArrListSize });

            charArrListSize++;
            symTableSize++;
        }
    }
}

void SemanticAnalyzer::addFunc()
{
    if (!defFuncReg.flag || mainFuncFlag)
        return;

    if (defFuncReg.reg.defRVType == "void" && defFuncReg.reg.hasRV == true)			// 错误g：无返回值的函数存在不匹配的return语句
        semErrorReg.push_back({ defFuncReg.rPointer, "g" });

    else if (defFuncReg.reg.defRVType != defFuncReg.reg.actRVType)					// 错误h：有返回值的函数缺少return语句或存在不匹配的return语句
        semErrorReg.push_back({ defFuncReg.rPointer, "h" });

    else
    {
        funcList.push_back({ defFuncReg.reg.defRVType, defFuncReg.reg.name, defFuncReg.reg.paraList, defFuncReg.reg.startPointer, defFuncReg.reg.fHead });
        SymTable.push_back({ "func", defFuncReg.reg.name, funcSize });

        funcSize++;
        symTableSize++;
    }
}

void SemanticAnalyzer::addIntReg(string type, string name, int value)
{
    bool isIfUnav = !conditionReg.empty() && conditionReg.back().condStateFlag && !conditionReg.back().conditionFlag && !conditionReg.back().ifStatus;
    bool isClosed = !ptrToCod->isClosedFlag.empty() && ptrToCod->isClosedFlag.back();

    if (expReg.empty())
        return;
    else if (isIfUnav && isClosed)
        return;

    if (expReg.back().intExpReg.empty() && expReg.back().charExpReg.empty())
        expReg.back().dataType = "int";

    if (expReg.back().dataType != "int")
    {
        expReg.back().flag = false;
        return;
    }

    expReg.back().intExpReg.push_back({ type, name, value });
}

void SemanticAnalyzer::addCharReg(string type, string name, char value)
{
    bool isIfUnav = !conditionReg.empty() && conditionReg.back().condStateFlag && !conditionReg.back().conditionFlag && !conditionReg.back().ifStatus;
    bool isClosed = !ptrToCod->isClosedFlag.empty() && ptrToCod->isClosedFlag.back();

    if (expReg.empty())
        return;
    else if (isIfUnav && isClosed)
        return;

    if (expReg.back().intExpReg.empty() && expReg.back().charExpReg.empty())
        expReg.back().dataType = "char";

    if (expReg.back().dataType != "char")
    {
        expReg.back().flag = false;
        return;
    }

    expReg.back().charExpReg.push_back({ type, name, value });
}

void SemanticAnalyzer::addRegInd(int index)
{
    bool isIfUnav = !conditionReg.empty() && conditionReg.back().condStateFlag && !conditionReg.back().conditionFlag && !conditionReg.back().ifStatus;
    bool isClosed = !ptrToCod->isClosedFlag.empty() && ptrToCod->isClosedFlag.back();

    if (isIfUnav && isClosed)
        return;
    else if (index >= symTableSize)
        return;

    string dataType = SymTable[index].listName;

    if (dataType == "int")
        addIntReg(intList[SymTable[index].index].type, SymTable[index].name, intList[SymTable[index].index].value);
    else if (dataType == "char")
        addCharReg(charList[SymTable[index].index].type, SymTable[index].name, charList[SymTable[index].index].value);
    else if (dataType == "intArray")
        addIntReg("var", SymTable[index].name, 0);
    else if (dataType == "charArray")
        addCharReg("var", SymTable[index].name, ' ');
}

void SemanticAnalyzer::regOperate(string optor)
{
    bool isIfUnav = !conditionReg.empty() && conditionReg.back().condStateFlag && !conditionReg.back().conditionFlag && !conditionReg.back().ifStatus;
    bool isClosed = !ptrToCod->isClosedFlag.empty() && ptrToCod->isClosedFlag.back();

    if (expReg.empty())
        return;
    else if (isIfUnav && isClosed)
        return;

    if (optor == "neg" && !expReg.back().intExpReg.empty())
        (expReg.back().intExpReg.end() - 1)->value = -(expReg.back().intExpReg.end() - 1)->value;

    else if (optor == "[]" && expReg.size() >= 2 && !expReg.back().intExpReg.empty())
    {
        if ((expReg.end() - 2)->dataType == "int")
            (expReg.end() - 2)->intExpReg.back().type = to_string(expReg.back().intExpReg.back().value);
        else if ((expReg.end() - 2)->dataType == "char")
            (expReg.end() - 2)->charExpReg.back().type = to_string(expReg.back().intExpReg.back().value);
    }

    else if (optor == "=")
    {
        if (expReg.back().dataType == "int" && expReg.back().intExpReg.size() >= 2)
        {
            (expReg.back().intExpReg.end() - 2)->value = (expReg.back().intExpReg.end() - 1)->value;

            string name1 = arrayNamePro("int", 1);
            string name2 = arrayNamePro("int", 2);
            ptrToCod->addFormula({ to_string(ptrToCod->lineStart), "=", name1, "_", name2 });

            expReg.back().intExpReg.pop_back();
        }

        else if (expReg.back().dataType == "char" && expReg.back().charExpReg.size() >= 2)
        {
            (expReg.back().charExpReg.end() - 2)->value = (expReg.back().charExpReg.end() - 1)->value;

            string name1 = arrayNamePro("char", 1);
            string name2 = arrayNamePro("char", 2);
            ptrToCod->addFormula({ to_string(ptrToCod->lineStart), "=", name1, "_", name2 });

            expReg.back().charExpReg.pop_back();
        }
    }

    else if (optor == "+" || optor == "-" || optor == "*" || optor == "/")
    {
        if (expReg.back().dataType == "int" && expReg.back().intExpReg.size() >= 2)
        {
            int result = (expReg.back().intExpReg.end() - 2)->value;

            if (optor == "+")
                result += (expReg.back().intExpReg.end() - 1)->value;
            else if (optor == "-")
                result -= (expReg.back().intExpReg.end() - 1)->value;
            else if (optor == "*")
                result *= (expReg.back().intExpReg.end() - 1)->value;
            else if (optor == "/")
                result /= (expReg.back().intExpReg.end() - 1)->value;

            string name1 = arrayNamePro("int", 2);

            ptrToCod->addFormula({ to_string(ptrToCod->lineStart), optor, name1, expReg.back().intExpReg.back().name, "t" + to_string(ptrToCod->tempVarDis()) });

            expReg.back().intExpReg.pop_back();
            expReg.back().intExpReg.pop_back();
            addIntReg("temp", "t" + to_string(ptrToCod->tempVarDis(false) - 1), result);
        }
    }
}

void SemanticAnalyzer::assignToElem(int index)
{
    if (!conditionReg.empty() && conditionReg.back().condStateFlag && !conditionReg.back().conditionFlag && !conditionReg.back().ifStatus)
        return;
    else if (index >= symTableSize)
        return;

    string type = SymTable[index].listName;

    if (type == "int" && !expReg.back().intExpReg.empty())
        intList[SymTable[index].index].value = (expReg.back().intExpReg.end() - 1)->value;
    else if (type == "char" && !expReg.back().charExpReg.empty())
        charList[SymTable[index].index].value = (expReg.back().charExpReg.end() - 1)->value;
    else if (type == "intArray" && !expReg.back().intExpReg.empty())
    {
        int i = atoi(expReg.back().intExpReg.back().type.c_str());

        if (i >= 0 && i < intArrList[SymTable[index].index].size)
            intArrList[SymTable[index].index].data[i] = expReg.back().intExpReg.back().value;
        else								// 错误p：数组下标越界
            addSemError({ synInputPtr, "p" });
    }
    else if (type == "charArray" && !expReg.back().charExpReg.empty())
    {
        int i = atoi(expReg.back().charExpReg.back().type.c_str());

        if (i >= 0 && i < charArrList[SymTable[index].index].size)
            charArrList[SymTable[index].index].data[i] = expReg.back().charExpReg.back().value;
        else								// 错误p：数组下标越界
            addSemError({ synInputPtr, "p" });
    }
}

void SemanticAnalyzer::arrValueToExpReg(int index)
{
    if (expReg.empty())
        return;

    if (expReg.back().dataType == "int")
    {
        int i = atoi(expReg.back().intExpReg.back().type.c_str());
        expReg.back().intExpReg.back().value = intArrList[index].data[i];
    }
    else if (expReg.back().dataType == "char")
    {
        int i = atoi(expReg.back().charExpReg.back().type.c_str());
        expReg.back().charExpReg.back().value = charArrList[index].data[i];
    }
}

string SemanticAnalyzer::getTopRegType()
{
    if (!expReg.empty() && !expReg.back().intExpReg.empty())
        return "int";
    else if (!expReg.empty() && !expReg.back().charExpReg.empty())
        return "char";
    else
        return "void";
}

int SemanticAnalyzer::getTopIntRegValue()
{
    int value = 0;

    if (!expReg.empty() && !expReg.back().intExpReg.empty())
        value = (expReg.back().intExpReg.end() - 1)->value;

    return value;
}

string SemanticAnalyzer::outputRegTop()
{
    if (!conditionReg.empty() && conditionReg.back().condStateFlag && !conditionReg.back().conditionFlag && !conditionReg.back().ifStatus)
        return "";

    string outName;
    bool flag1 = callFuncReg.empty() || !callFuncReg.back().flag;
    bool flag2 = !mainFuncFlag;
    if (flag1 && flag2)
        return "";

    string regType = getTopRegType();

    if (regType == "int" && !expReg.back().intExpReg.empty())
    {
        cout << (expReg.back().intExpReg.end() - 1)->value;
        outName = arrayNamePro("int", 1);
    }
    else if (regType == "char" && !expReg.back().charExpReg.empty())
    {
        cout << (expReg.back().charExpReg.end() - 1)->value;
        outName = arrayNamePro("int", 1);
    }

    return outName;
}

void SemanticAnalyzer::outputString(string outputs)
{
    if (!conditionReg.empty() && conditionReg.back().condStateFlag && !conditionReg.back().conditionFlag && !conditionReg.back().ifStatus)
        return;

    bool flag1 = !callFuncReg.empty() && callFuncReg.back().flag;
    bool flag2 = mainFuncFlag;

    if (flag1 || flag2)
        cout << outputs;
}

void SemanticAnalyzer::inputVar(string name)
{
    if (!conditionReg.empty() && conditionReg.back().condStateFlag && !conditionReg.back().conditionFlag && !conditionReg.back().ifStatus)
        return;

    bool flag1 = callFuncReg.empty() || !callFuncReg.back().flag;
    bool flag2 = !mainFuncFlag;
    if (flag1 && flag2)
        return;

    int index = getSymIndex(name);

    if (index < symTableSize)
    {
        if (SymTable[index].listName == "int")
        {
            int temp;
            cin >> temp;
            intList[SymTable[index].index].value = temp;
        }

        else if (SymTable[index].listName == "char")
        {
            char temp;
            cin >> temp;
            intList[SymTable[index].index].value = temp;
        }
    }
}

void SemanticAnalyzer::addExpReg()
{
    expRegElem exp;

    exp.flag = true;
    exp.dataType = "void";

    expReg.push_back(exp);
}

void SemanticAnalyzer::deleteExpReg()
{
    if (!expReg.empty())
        expReg.pop_back();
}

void SemanticAnalyzer::addCondReg()
{
    ifRegElem reg;

    reg.condStateFlag = true;
    reg.conditionFlag = false;
    reg.ifStatus = false;
    reg.ifReg = 0;

    conditionReg.push_back(reg);
}

void SemanticAnalyzer::deleteCondReg()
{
    if (!conditionReg.empty())
        conditionReg.pop_back();
}

void SemanticAnalyzer::setIfFlag(string RelOptor)
{
    bool isBinOptor = true;

    if (RelOptor == "")
    {
        if (conditionReg.back().ifReg != 0)
            conditionReg.back().ifStatus = true;

        return;
    }

    else if (expReg.empty())
    {
        conditionReg.back().ifStatus = false;
        return;
    }

    else if (RelOptor == "@")
        isBinOptor = false;

    else if (RelOptor == "==" && conditionReg.back().ifReg == expReg.back().intExpReg.back().value)
        conditionReg.back().ifStatus = true;

    else if (RelOptor == "!=" && conditionReg.back().ifReg != expReg.back().intExpReg.back().value)
        conditionReg.back().ifStatus = true;

    else if (RelOptor == ">" && conditionReg.back().ifReg > expReg.back().intExpReg.back().value)
        conditionReg.back().ifStatus = true;

    else if (RelOptor == "<" && conditionReg.back().ifReg < expReg.back().intExpReg.back().value)
        conditionReg.back().ifStatus = true;

    else if (RelOptor == ">=" && conditionReg.back().ifReg >= expReg.back().intExpReg.back().value)
        conditionReg.back().ifStatus = true;

    else if (RelOptor == "<=" && conditionReg.back().ifReg <= expReg.back().intExpReg.back().value)
        conditionReg.back().ifStatus = true;

    else
        conditionReg.back().ifStatus = false;

    if (isBinOptor)
        ptrToCod->addFormula({ to_string(ptrToCod->lineStart), "j" + RelOptor, ifNameReg, expReg.back().intExpReg.back().name, "[" + to_string(ptrToCod->lineStart + 2) + "]" });
    else
        ptrToCod->addFormula({ to_string(ptrToCod->lineStart), "jnz", ifNameReg, "_", "[" + to_string(ptrToCod->lineStart + 2) + "]" });

    if (!ptrToCod->LHReg.empty())
    {
        ptrToCod->LHReg.back().push_back(ptrToCod->lineStart);
        ptrToCod->addFormula({ to_string(ptrToCod->lineStart), "j", "_", "_", "100" });
    }
}

void SemanticAnalyzer::resetIfReg()
{
    this->conditionReg.pop_back();
}

void SemanticAnalyzer::sendError()
{
    int semErrorSize = semErrorReg.size();
    int lineSize = ptrToErr->lineNum.size();

    for (int i = 0, line = 0; i < semErrorSize; i++)
    {
        while (line < lineSize && semErrorReg[i].lineInd >= ptrToErr->lineNum[line])
            line++;

        ptrToErr->errorList.push_back({ line , semErrorReg[i].errorType });
    }
}

string SemanticAnalyzer::getElemType(string name)
{
    bool isFound = false;
    string type = "none";

    for (int i = 0; !isFound && i < intListSize; i++)
        if (intList[i].name == name)
        {
            isFound = true;
            type = intList[i].type;
            break;
        }

    for (int i = 0; !isFound && i < charListSize; i++)
        if (charList[i].name == name)
        {
            isFound = true;
            type = charList[i].type;
            break;
        }

    for (int i = 0; !isFound && i < intArrListSize; i++)
        if (intArrList[i].name == name)
        {
            isFound = true;
            type = "var";
            break;
        }

    for (int i = 0; !isFound && i < charArrListSize; i++)
        if (charArrList[i].name == name)
        {
            isFound = true;
            type = "var";
            break;
        }

    return type;
}

int SemanticAnalyzer::getSymSize()
{
    return symTableSize;
}

int SemanticAnalyzer::getSymIndex(string name)
{
    int index = symTableSize;
    bool isFound = false;

    int globalStart = 0;
    int globalEnd = mainFuncPointer;
    if (!funcList.empty())
        globalEnd = funcList.front().startPointer;

    int localStart = mainFuncPointer;
    int localEnd = symTableSize;

    bool flag1 = !callFuncReg.empty() && callFuncReg.back().flag && !funcList.empty() && !VParaListFlag;
    bool flag2 = ((int)callFuncReg.size() >= 2) && callFuncReg.back().flag && !funcList.empty() && VParaListFlag;

    int offset = 0;
    if (flag1)
        offset = 0;
    else if (flag2)
        offset = 1;

    if (flag1 || flag2)
    {
        int index = getFuncIndex(offset);

        localStart = funcList[index].startPointer;

        if ((index + 2) <= (int)funcList.size())
            localEnd = funcList[index + 1].startPointer;
        else
            localEnd = mainFuncPointer;
    }


    for (int i = globalStart; !isFound && i < globalEnd; i++)
        if (SymTable[i].name == name)
        {
            index = i;
            isFound = true;
            break;
        }
    for (int i = localStart; !isFound && i < localEnd; i++)
        if (SymTable[i].name == name)
        {
            index = i;
            isFound = true;
            break;
        }

    return index;
}

void SemanticAnalyzer::addSemError(errorPair error)
{
    semErrorReg.push_back(error);
}

void SemanticAnalyzer::resetDefFuncReg()
{
    this->defFuncReg.rPointer = 0;
    this->defFuncReg.reg.defRVType = "void";
    this->defFuncReg.reg.actRVType = "void";
    this->defFuncReg.reg.hasRV = false;
    this->defFuncReg.reg.name = "";
    this->defFuncReg.reg.paraList.clear();
    this->defFuncReg.reg.paraCounter = 0;
    this->defFuncReg.reg.startPointer = 0;
    this->defFuncReg.reg.fHead = 0;
}

void SemanticAnalyzer::addCallFuncReg()
{
    FuncReg Reg;

    Reg.flag = true;
    Reg.rPointer = 0;

    Reg.reg.actRVType = "void";
    Reg.reg.defRVType = "void";
    Reg.reg.fHead = 0;
    Reg.reg.hasRV = false;
    Reg.reg.name = "";
    Reg.reg.paraCounter = 0;
    Reg.reg.startPointer = 0;

    callFuncReg.push_back(Reg);
}

void SemanticAnalyzer::deleteCallFuncReg()
{
    if (!callFuncReg.empty())
        callFuncReg.pop_back();
}

void SemanticAnalyzer::addParaElem(string type, string name)
{
    if (!defFuncReg.flag)
        return;

    /*
    if (type == "int")
        defFuncReg.reg.paraList.push_back({ type, name, intListSize });
    else if (type == "char")
        defFuncReg.reg.paraList.push_back({ type, name, charListSize });
    */

    defFuncReg.reg.paraList.push_back({ type, name, symTableSize });
}

int SemanticAnalyzer::getFuncIndex(int offset)
{
    int index;

    for (index = 0; index < funcSize; index++)
        if (!callFuncReg.empty() && funcList[index].name == (callFuncReg.end() - 1 - offset)->reg.name)
            break;

    return index;
}

FuncElem SemanticAnalyzer::getFunc(int index)
{
    if (index < (int)funcList.size())
        return funcList[index];

    else
        return funcList.front();
}

bool SemanticAnalyzer::callFuncCheck(int pointer, int index)
{
    if (callFuncReg.empty() || !callFuncReg.back().flag)
        return false;


    if (index == funcSize)							// 错误c：未定义的名字
    {
        semErrorReg.push_back({ pointer, "c" });
        return false;
    }

    else
    {
        if ((int)funcList[index].paraList.size() != callFuncReg.back().reg.paraCounter)		// 错误d：函数参数个数不匹配
        {
            semErrorReg.push_back({ pointer, "d" });
            return false;
        }

        for (unsigned int i = 0; i < funcList[index].paraList.size(); i++)
            if (funcList[index].paraList[i].listName != callFuncReg.back().reg.paraList[i].listName)
            {
                semErrorReg.push_back({ pointer, "e" });						// 错误e：函数参数类型不匹配
                return false;
            }
    }

    return true;
}

string SemanticAnalyzer::arrayNamePro(string dataType, int offset)
{
    string name;

    if (dataType == "int")
    {
        name = (expReg.back().intExpReg.end() - offset)->name;

        /*
        if ((expReg.back().intExpReg.end() - offset)->type[0] >= '0' && (expReg.back().intExpReg.end() - offset)->type[0] <= '9')
            name += "[" + (expReg.back().intExpReg.end() - offset)->type + "]";
        */
    }

    else if (dataType == "char")
    {
        name = (expReg.back().charExpReg.end() - offset)->name;

        /*
        if ((expReg.back().charExpReg.end() - offset)->type[0] >= '0' && (expReg.back().charExpReg.end() - offset)->type[0] <= '9')
            name += "[" + (expReg.back().charExpReg.end() - offset)->type + "]";
        */
    }

    return name;
}
