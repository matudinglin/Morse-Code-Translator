#include "codegeneration.h"

CodeGenerator::CodeGenerator()
{
    tempVarCounter = 0;
    lineStart = 100;
}

void CodeGenerator::addFormula(QuaFormula formula)
{
    if (isClosedFlag.empty() || !isClosedFlag.back())
    {
        formulaList.push_back(formula);
        lineStart++;
    }
}

string CodeGenerator::getFormula()
{
    string output;
    int formulaSize = (int)formulaList.size();

    if (formulaSize > 0)
        output += formulaList[0].lineNum + "    (" + formulaList[0].optor + ", " +  formulaList[0].operand1 + ", " + formulaList[0].operand2 + ", " + formulaList[0].dest + ")";
    for (int i = 1; i < formulaSize; i++)
        output += "\n" + (formulaList[i].lineNum + "    (" + formulaList[i].optor + ", " + formulaList[i].operand1 + ", " + formulaList[i].operand2 + ", " + formulaList[i].dest + ")");

    return output;
}

void CodeGenerator::addLHReg()
{
    vector<int> reg;

    LHReg.push_back(reg);
}

void CodeGenerator::deleteLHReg()
{
    if (!LHReg.empty())
        LHReg.pop_back();
}

void CodeGenerator::addClosedFlag()
{
    isClosedFlag.push_back(true);
}

void CodeGenerator::deleteClosedFlag()
{
    if (!isClosedFlag.empty())
        isClosedFlag.pop_back();
}

int CodeGenerator::tempVarDis(bool isInc)
{
    int VarNum = tempVarCounter;

    if (isClosedFlag.empty() || !isClosedFlag.back())
        tempVarCounter += (int)isInc;

    return VarNum;
}

void CodeGenerator::addIfJump(int offset)
{
    if (!LHReg.empty() && !LHReg.back().empty() && (int)formulaList.size() > LHReg.back().back() - 100)
    {
        formulaList[LHReg.back().back() - 100].dest = "[" + to_string(lineStart + offset) + "]";
        LHReg.back().pop_back();
    }
}

void CodeGenerator::addWhileJump()
{
    if (!LHReg.empty() && !LHReg.back().empty() && formulaList.size() >= 2)
    {
        formulaList.back().dest = "[" + to_string(LHReg.back().back()) + "]";
        LHReg.back().pop_back();
    }
}

void CodeGenerator::addDoWhileJump()
{
    if (!formulaList.empty())
        formulaList.pop_back();
    if (!LHReg.empty() && !LHReg.back().empty())
        LHReg.back().pop_back();

    if (!formulaList.empty() && !LHReg.empty() && !LHReg.back().empty())
    {
        formulaList.back().dest = "[" + to_string(LHReg.back().back()) + "]";
        LHReg.back().pop_back();
    }
}

void CodeGenerator::addForJump()
{
    addIfJump(1);
    addFormula({ to_string(lineStart), "j", "_", "_", "100" });

    if (!LHReg.empty() && !LHReg.back().empty())
    {
        formulaList.back().dest = "[" + to_string(LHReg.back().back()) + "]";
        LHReg.back().pop_back();
    }
}

void CodeGenerator::addDirectJump()
{
    if (!LHReg.empty())
        LHReg.back().push_back(lineStart);

    addFormula({ to_string(lineStart), "j", "_", "_", "100" });
}
