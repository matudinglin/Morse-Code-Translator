#ifndef ERRORANALYZER_H
#define ERRORANALYZER_H

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



#endif
