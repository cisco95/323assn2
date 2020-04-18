//Hammad Qureshi, Francisco Ramirez, and Omar Ramirez
//CPSC 323
//Assignment 1

#pragma once
#include <fstream>
#include <iostream>
#include <string>
#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <stdio.h>
#include <string.h>

using namespace std;

enum FSMTransitions {
	Ignore = 0,
	Integer,
	Real,
	String,
	Operator,
	Unknown,
	Space
};


//Finite State Machine Table
int stateTable[7][7] = { {0,  Integer, Real, String, Operator, Unknown, Space},
		        {Integer,   Integer, Real, Ignore, Ignore, Ignore, Ignore},
		        {Real,         Real, Unknown, Ignore, Ignore, Ignore, Ignore},
			   {String,     String, Ignore, String, String, Unknown, Ignore},
		        {Operator,   Ignore, Ignore, String, Ignore, Ignore, Ignore},
			   {Unknown,   Unknown, Unknown, Unknown, Unknown, Unknown, Ignore},
			   {Space,      Ignore, Ignore, Ignore, Ignore, Ignore, Ignore}};

char keywords[20][15] = { "int", "float", "bool", "true", "false", "if", "else", "then", "endif", "while",
				  "whileend", "do", "doend", "for", "forend", "input", "output", "and", "or" , "not" };

struct lexeme
{
	string lex; //String or char? Should be a string according to professor since we are taking in words from the text document
	int lexNumber = 0; //lexeme
	string token; //This will hold text coming from the file
};

ifstream file("Input.txt");
ofstream myFile;
ofstream tokenFile;

int flag = 0;
int lineNumber = 1;
int stackIndex = 0;
char testChar = ' ';
char	stack[20] = " ";
char testWord[20];
char testCharList[20];
bool status; //conditionSet

bool isKeyword(char input[]) {
  for (int i = 0; i < 32; ++i) {
    if (strcmp(keywords[i], input) == 0) {
      return true;
    }
  }
  return false;
}


void Error(string str)
{
  //Clear output file
  myFile.close();
  tokenFile.close();
  myFile.open("LexOutput.txt", ios::out | ios::trunc);
  tokenFile.open("tokens.txt", ios::out | ios::trunc);
  // Output error message
  myFile << "Error: " << str << " line number: " << lineNumber;
  tokenFile << "Error: " << str << " line number: " << lineNumber;
  exit(EXIT_FAILURE);
}

class Lex {
private:
	//For Lexical Analyzer
	lexeme *lexArr = new lexeme[1000000]; //Create a type lexeme array to store data from the text file
	int countWord; //Acts as the index for the lexArr array



public:


	//***********LEXICAL ANALYZER CODE*********************//
	void lexer(int &x) {
		bool print = false;
		bool	printline = false;
		bool test = false;
		char ops[] = "+-*/%=";
		char	seps[] = "'(){}[],.:;!";
		int i;





		//discard all spaces
		while (testChar == ' ') {
			testChar = file.get();
		}

		//check if comment
		if (testChar == '!') {
			testChar = file.get();
			while (testChar != '!') {
				testChar = file.get();
			}
			return;
		}

		//Check operator
		for (i = 0; i < 6; ++i)
		{
			if (testChar == ops[i]) {
				//cout << testChar << " is operator\n";
				myFile << "\n\nToken:\tOperator" << "\t\tLexeme:\t" << testChar;
        tokenFile << "Operator" << endl;
				if (printline)
					myFile << " at line " << lineNumber;
				myFile << endl;
				if (print)
					cout << "\nToken:\tOperator" << "\t\tLexeme:\t" << testChar << endl;
				flag = 1;
				return;
			}
		}

		//Check seprator
		for (int m = 0; m < 11; m++)
		{
			if (testChar == seps[m]) {
				//cout << testChar << " is separator\n";
				myFile << "\n\nToken:\tSeparator" << "\t\tLexeme:\t" << testChar;
        tokenFile << "Separator" << endl;

				if (printline)
					myFile << " at line " << lineNumber;
				myFile << endl;
				if (print)
					cout << "\nToken:\tSeparator" << "\t\tLexeme:\t" << testChar << endl;
				flag = 1;
				return;
			}
		}

		//Check if number
		if (isdigit(testChar))
		{
			//cout << testChar << " is a number\n";
			char str[5];
			int x = 0;
			testWord[x] = '\0';
			x = 0;
			str[x] = testChar;

			while (isdigit(file.peek())) {
				testChar = file.get();
				str[x] = testChar;
				x++;
			}

			myFile << "\n\nToken:\tNumber" << "\t\t\tLexeme:\t";
      tokenFile << "Number" << endl;

			if (x == 0)
				myFile << str[0];
			else {
				for (int u = 0; u < x; u++) {
					testWord[x] = '\0';
					myFile << str[u];
				}
			}
			if (printline)
				myFile << " at line " << lineNumber;
			myFile << endl;
			flag = 0;
			return;
		}

		//If the first character is not operator nor the seperator, get all the rest of word until reach space
		if (isalnum(testChar) || testChar == '$')
		{
			//cout << "IS ALNUM: " << testChar << "\tj = " << j << endl;
			bool stop = false;
			char next;
			if (testChar == ' ' || testChar == '\n')
				stop = true;
			while (!stop) {
				testWord[x++] = testChar;
				next = file.peek();
				for (i = 0; i < 11; ++i)
				{
					if (next == seps[i]) {
						stop = true;
					}
				}
				for (i = 0; i < 6; ++i)
				{
					if (next == ops[i]) {
						stop = true;
					}
				}
				if (!stop)
					testChar = file.get();
				if (testChar == ' ' || testChar == '\n')
					stop = true;
			}

			test = true;
			//Make the rest of c string become null
			testWord[x] = '\0';
			//reset new index of testWord for next testWord
			x = 0;


			if (isKeyword(testWord)) {
				myFile << "\n\nToken:\tKeyword" << "\t\t\tLexeme:\t" << testWord;
        tokenFile << "Keyword" << endl;

				if (printline)
					myFile << " at line " << lineNumber;
				myFile << endl;
				if (print)
					cout << "\nToken:\tKeyword" << "\t\t\tLexeme:\t" << testWord << endl;
			}
			else {
				myFile << "\n\nToken:\tIdentifier" << "\t\tLexeme:\t" << testWord;
        tokenFile << "Identifier" << endl;

				if (printline)
					myFile << " at line " << lineNumber;
				myFile << endl;
				if (print)
					cout << "\nToken:\tIdentifier" << "\t\tLexeme:\t" << testWord << endl;
			}
			flag = 0;
		}
		//cout << test << endl;
	}

	//This is the function which the user calls to run the program
	int LexAnalyze(string nameOfFile, string nameOfFile2) {
		myFile.open(nameOfFile);
    tokenFile.open(nameOfFile2);

		if (!file.is_open()) {
			//cout << "Error while opening the file, please change file name to SampleInputFile\n";
			cout << "Exited the program";
			exit(0);
		}

		int i = 0;
		int x = 0;
		while (!file.eof())
		{
			testChar = file.get();
			testCharList[i] = testChar;
			i++;
			if (testChar == '\n')
			{
				lineNumber++;
				for (int i = 0; i < 20; i++)
					testCharList[i] = '\0';
				i = 0;
			}
			lexer(x);
		}
		if (stackIndex != 0) {
			Error("The closing argument was not found");
		}

		file.close();
		myFile.close();
    tokenFile.close();
		return 0;
	}
};
