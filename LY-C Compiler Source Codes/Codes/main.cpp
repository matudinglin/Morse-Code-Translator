#include<iostream>
#include<string>
#include"SyntaxAnalysis.h"
using namespace std;

int main()
{
	string inFileName = "code3.txt";
	string synFileName = "syn.txt";
	string errorFileName = "err.txt";
	string quaFileName = "quaternion.txt";
	string lexFileName = "lex.txt";
	
	
	string input;
	string output;

	ErrorAnalyzer err;
	LexicalAnalyzer lex(&err);

	//input
	ifstream infile(inFileName);
	char ch;
	infile.get(ch);
	while (!infile.eof())
	{	
		input += ch;
		infile.get(ch);
	}
	infile.close();

	// Lexical Analysis
	lex.parse(input);
	lex.searchError();

	// Code Generation
	CodeGenerator cod;

	SemanticAnalyzer sem(&err, &cod);
	SyntaxAnalyzer syn(&err, &lex, &sem, &cod);
	int pointer = 0;

	// Syntax & Semantic Analysis
	bool flag = syn.isProgram(pointer);
	syn.sendError();
	sem.sendError();

	//cout << flag;

	// Output results
	output = syn.getOutputs();

	//output
	ofstream outfile0(lexFileName);
	outfile0 << lex.getResult();
	outfile0.close();

	ofstream outfile1(synFileName);
	outfile1 << output;
	outfile1.close();

	output = cod.getFormula();

	ofstream outfile2(quaFileName);
	outfile2 << output;
	outfile2.close();

	err.sortErrors();
	output = err.getErrors();

	ofstream outfile3(errorFileName);
	outfile3 << output;
	outfile3.close();
}