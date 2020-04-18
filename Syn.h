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

ifstream fileIn("input.txt");
ofstream myFileOut;

class Syn {
private:
	//For Lexical Analyzer
	lexeme *lexArr = new lexeme[1000000]; //Create a type lexeme array to store data from the text file
	int countWord; //Acts as the index for the lexArr array

	//For Syntax Analyzer
	int flag = 0;
	int lineNumber = 1;
	int stackIndex = 0;
	char testChar = ' ';
	char	stack[20] = " ";
	char testWord[20];
	char testCharList[20];
	bool status; //conditionSet

public:



	//Does a comparison with character to the keyword array to see if there is a keyword
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
		myFileOut.close();
		myFileOut.open("output.txt", ios::out | ios::trunc);
		// Output error message
		myFileOut << "Error: " << str << " line number: " << lineNumber;
		exit(EXIT_FAILURE);
	}

	string Identifiers() { //Identifiers - syntaxID
		string null_str;
		char operators[] = "+-*/%";
		bool RHS = true; //rightHandSide
		bool firstOfLHS = true; //firstofLHS

		for (size_t i = 0; i < strlen(testCharList); i++)
		{
			if (testCharList[i] == '=') { RHS = false; }
		}

		if (!RHS)
		{
			for (size_t i = 0; i < strlen(testCharList); i++)
			{
				for (int j = 0; j < 5; j++) {
					if (testCharList[i] == operators[j]) { firstOfLHS = false; }
				}
			}
			if (firstOfLHS) { myFileOut << "<Expression> -> <Term> <Expression Prime>\n"; }

			myFileOut << "<Term> -> <Factor><TermPrime>\n";
			myFileOut << "<Factor> -> <Identifier>\n";
		}
		else
		{
			myFileOut << "<Statement> -> <Assign>\n";
			myFileOut << "<Assign> -> <Identifier> = <Expression>\n";
		}
		return null_str;
	}

	string Separators() { //Separators - syntaxSep
		string phrase = " <Separator> -> ";

		char openers[5] = { "([{'" }, closers[] = { ")]}'" };
		//openers[5] += '"';
		for (int a = 0; a < 4; a++) {
			if (testChar == openers[a] && stack[stackIndex] != openers[a]) {//know testChar is a closing separator
				stackIndex++;
				stack[stackIndex] = testChar;//add separator to the stack

				if (testChar == '(') {
					phrase += " <Condition>\n";
					phrase += " <Condition> -> <StatementList>";
				}
				else {
					phrase += " <OpeningSeparator> <StatementList>\n";
					phrase += " <OpeningSeparator> -> " + testChar;
				}
				return phrase;
			}
			else if (testChar == closers[a]) {//know testChar is a closing separator
				if (openers[a] == stack[stackIndex]) {
					//continue
					stack[stackIndex] = ' ';
					stackIndex--;
					if (testChar == ')') {
						phrase += " <Condition>\n";
						phrase += " <Condition> -> <StatementList> )";
						phrase += "\n <StatementList> -> Epsilon";
					}
					else {
						phrase += " <StatementList> <ClosingSeparator>\n";
						phrase += " <ClosingSeparator> -> " + testChar;
					}
					return phrase;
				}
				else {
					Error("Closing separator incompatible");
				}
			}
		}
		phrase += " <EndSeparator>\n";
		return phrase;
	}

	string Keywords() { //Keywords - syntaxKey
		string phrase;

		char parenthesisWords[10][10] = { "if", "while", "for", "forend","function", "main" }; //words With Parentheses
		for (int i = 0; i < 7; i++) {
			if (strcmp(testWord, parenthesisWords[i]) == 0)
				status = true;
		}

		for (int i = 0; i < 20; i++) {
			if (strcmp(testWord, keywords[i]) == 0) {
				string key = (string)keywords[i];

				if (i < 3) { //is a variable
					phrase += " <KeyWord> -> <Variable>\n";
					phrase += " <Variable> -> <" + key + ">";
				}
				else if (i > 2 && i < 13) {//is a conditional
					phrase += " <KeyWord> -> <Conditional>\n";
					phrase += " <Conditional> -> <" + key + "> + <Separator>";
				}
				else {//is a function
					phrase += " <KeyWord> -> <Function>\n";
					phrase += " <Function> -> <" + key + ">";
				}
			}
		}
		return phrase;
	}

	string numbers() { //Numbers - syntaxNum
		string num;
		num = " <Number> -> <Assign>";
		num = " <Assign> -> <" + (string)testWord + ">";
		return num;
	}

	string operators() { //Operators - syntaxOp
		string null_str;
		if (testChar == '*') {
			myFileOut << " <TermPrime> -> * <Factor> <TermPrime>\n";
			myFileOut << " <ExpressionPrime> -> <Empty>\n";
		}
		else if (testChar == '/') {
			myFileOut << " <TermPrime> -> / <Factor> <TermPrime>\n";
			myFileOut << " <ExpressionPrime> -> <Empty>\n";
		}
		else if (testChar == '+') {
			myFileOut << " <TermPrime> -> <Empty>\n";
			myFileOut << " <ExpressionPrime> -> + <Term> <ExpressionPrime>\n";
		}
		else if (testChar == '-') {
			myFileOut << " <TermPrime> -> <Empty>\n";
			myFileOut << " <ExpressionPrime> -> - <Term> <ExpressionPrime>\n";
		}
		myFileOut << " <Empty>->Epsilon\n";

		return null_str;
	}

	//***********SYNTAX ANALYZER CODE*********************//
  void syner(int &x) {
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
        myFileOut << "\n\nToken:\tOperator" << "\t\tLexeme:\t" << testChar;
        if (printline)
          myFileOut << " at line " << lineNumber;
        myFileOut << endl;
        if (print)
          cout << "\nToken:\tOperator" << "\t\tLexeme:\t" << testChar << endl;
        myFileOut << operators();
        flag = 1;
        return;
      }
    }

    //Check seprator
    for (int m = 0; m < 11; m++)
    {
      if (testChar == seps[m]) {
        //cout << testChar << " is separator\n";
        myFileOut << "\n\nToken:\tSeparator" << "\t\tLexeme:\t" << testChar;
        if (printline)
          myFileOut << " at line " << lineNumber;
        myFileOut << endl;
        if (print)
          cout << "\nToken:\tSeparator" << "\t\tLexeme:\t" << testChar << endl;
        myFileOut << Separators();
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

      myFileOut << "\n\nToken:\tNumber" << "\t\t\tLexeme:\t";
      if (x == 0)
        myFileOut << str[0];
      else {
        for (int u = 0; u < x; u++) {
          testWord[x] = '\0';
          myFileOut << str[u];
        }
      }
      if (printline)
        myFileOut << " at line " << lineNumber;
      myFileOut << endl;
      flag = 0;
      myFileOut << numbers();
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
        myFileOut << "\n\nToken:\tKeyword" << "\t\t\tLexeme:\t" << testWord;
        if (printline)
          myFileOut << " at line " << lineNumber;
        myFileOut << endl;
        if (print)
          cout << "\nToken:\tKeyword" << "\t\t\tLexeme:\t" << testWord << endl;
        myFileOut << Keywords();
      }
      else {
        myFileOut << "\n\nToken:\tIdentifier" << "\t\tLexeme:\t" << testWord;
        if (printline)
          myFileOut << " at line " << lineNumber;
        myFileOut << endl;
        if (print)
          cout << "\nToken:\tIdentifier" << "\t\tLexeme:\t" << testWord << endl;
        myFileOut << Identifiers();
      }
      flag = 0;
    }
    //cout << test << endl;
  }



	//This is the function which the user calls to run the program
	int SynAnalyze(string nameOfFile ) {
		myFileOut.open(nameOfFile);

		if (!fileIn.is_open()) {
			//cout << "Error while opening the file, please change file name to SampleInputFile\n";
			cout << "Exited the program";
			exit(0);
		}

		int i = 0;
		int x = 0;
		while (!fileIn.eof())
		{
			testChar = fileIn.get();
			testCharList[i] = testChar;
			i++;
			if (testChar == '\n')
			{
				lineNumber++;
				for (int i = 0; i < 20; i++)
					testCharList[i] = '\0';
				i = 0;
			}
			syner(x);
		}
		if (stackIndex != 0) {
			Error("The closing argument was not found");
		}

		fileIn.close();
		myFileOut.close();
		return 0;
	}
};
