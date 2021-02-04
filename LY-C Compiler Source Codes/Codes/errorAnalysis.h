#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <unordered_map>
#include <algorithm>
#include <vector>
using namespace std;

struct errorPair
{
	int lineInd;
	string errorType;
};

class ErrorAnalyzer
{
public:
	vector<errorPair> errorList;
	vector<int> lineNum;

	void sortErrors();
	string getErrors();
};

bool compare(const errorPair& eP1, const errorPair& eP2)
{
	return eP1.lineInd < eP2.lineInd;
}

void ErrorAnalyzer::sortErrors()
{
	sort(errorList.begin(), errorList.end(), compare);
}

string ErrorAnalyzer::getErrors()
{
	string output;
	int errorSize = errorList.size();

	for (int i = 0; i < errorSize; i++)
	{
		if (!i)
			output += to_string(errorList[i].lineInd) + " " + errorList[i].errorType;

		else
			output += "\n" + to_string(errorList[i].lineInd) + ' ' + errorList[i].errorType;
	}

	return output;
}
