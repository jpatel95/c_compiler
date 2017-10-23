/*
	Jimmy Patel
	Professor Atkinson
	Compilers Lab Tuesday, 5:15
	1/10/15

	Phase I: Lexical Analysis
*/

/*
The following program is a lexical analyzer for a Simple C language. It take in input from
standard in and outputs the tokens with its correct categories in standard out.

The main function has a while loop that reads input from stdin character by character until
reaching the end of file.
	The characters read are split into 6 different cases using if-else statements:
		1. Identifier or Keyword - By an understore or alphabetical letter
		2. Numbers - By a digit
		3. Operators - By an operator (checks against the two checkIfOperator functions)
		4. Character - By a single quote
		5. String - By a double quote
		6. Whitespace - If the character does not fit the 5 cases above, it must be a whitespace
*/

#include <iostream>
#include <stdio.h>
#include <cctype>
#include <string>

using namespace std;

bool checkIfSingleOperator(char c);
bool checkIfDoubleOperator(string str);
bool checkIfKeyword(string str);

int main(void) {
	string strBuffer;
	char characterRead = cin.get();

	//While loop that continues until the end of file
	while(characterRead != EOF){
		//Case 1: - Identifier or Keyword if the current characterRead starts with an underscore or alpha
		if(characterRead=='_' || isalpha(characterRead)){
			strBuffer.clear();
			//Continues looping until encountering an operator or whitespace
			do{
				strBuffer += characterRead;
				characterRead=cin.get();
			} while(!isspace(characterRead) && !(checkIfSingleOperator(characterRead)));

			//If it is a keyword, print it
			if(checkIfKeyword(strBuffer)){
				cout<<"keyword:"<< strBuffer << endl;
			}
			else{
				//If it was not a keyword then it must be an identifier, print it
				cout<<"identifier:"<< strBuffer << endl;
			}
		}
		//Case 2: - Number if the current characterRead starts with a digit, it is a number
		else if(isdigit(characterRead)){
			strBuffer.clear();
			//Continues looping as long as there are still digits
			do{
				strBuffer+=characterRead;
				characterRead = cin.get();
			} while(isdigit(characterRead));
			cout<<"number:"<< strBuffer << endl;
		}
		//Case 3: - Operator if the current characterRead is a pipe or an operator from the list
		else if(checkIfSingleOperator(characterRead) || characterRead=='|'){ 
			strBuffer.clear();
			strBuffer+=characterRead;
			//Checks the next character and sees if both characters are an operator
			characterRead = cin.get();
			string doubleOperator = strBuffer + characterRead;
			if(checkIfDoubleOperator(doubleOperator)){
					strBuffer+=characterRead;
					characterRead=cin.get();
			}
			//Checks if the two characters were a block comment
			if(doubleOperator=="/*"){
				strBuffer.clear();
				//Loops until the closing block comment and does NOT print anything
				char prevChar;
				bool isEndingFound = false;
				do{
					prevChar=characterRead;
					characterRead=cin.get();
					if(characterRead=='/'){
						if(prevChar=='*'){
							isEndingFound = true;
						}
					}
				} while(!isEndingFound);
				characterRead=cin.get();
			}
			// A single '|' is not an operator
			else if(strBuffer!="|"){
				//Only prints if it was not a comment
				cout<<"operator:"<< strBuffer << endl;
			}
		}
		//Case 4: - Character if the current characterRead is a single quote
		else if(characterRead=='\''){
			strBuffer.clear();
			strBuffer+=characterRead;
			//Loops until the ending single quote, considers escape incase char is a '
			char prevChar;
			bool isEndingFound = false;
			do{
				prevChar=characterRead;
				characterRead=cin.get();
				if(characterRead=='\''){
					isEndingFound = true;
					if(prevChar=='\\'){
						isEndingFound = false;
					}
				}
				strBuffer+=characterRead;
			} while(!isEndingFound);
			characterRead=cin.get();
			//Prints out the character
			cout<<"character:"<< strBuffer << endl;
		}
		//Case 5: - String if the current characterRead is a double quote
		else if(characterRead=='"'){
			strBuffer.clear();
			strBuffer+=characterRead;
			//Loops until the ending double quote, considers escape incase cases
			char prevChar;
			bool isEndingFound = false;
			do{
				prevChar=characterRead;
				characterRead=cin.get();
				if(characterRead=='"'){
					isEndingFound = true;
					if(prevChar=='\\'){
						isEndingFound = false;
					}
				}
				strBuffer+=characterRead;
			} while(!isEndingFound);
			characterRead=cin.get();
			//Prints out the string
			cout<<"string:"<< strBuffer << endl;
		}
		//Case 6: If the characterRead does not fit the cases above, its a whitespace
		else {
			characterRead=cin.get();
		}
	}
	return 0;
}

/*
Function that takes in a string as a parameter and returns a boolean whether or not the string is a keyword.
*/
bool checkIfKeyword(string str){
	int legthOfArray = 32;
	string keywordsArray[] = {"auto", "break", "case", "char", "const", "continue", "default",
					"do", "double", "else", "enum", "extern", "float", "for", "goto", "if",
					"int", "long", "register", "return", "short", "signed", "sizeof", "static",
					"struct", "switch", "typedef", "union", "unsigned", "void", "volatile", "while"}; 
	
	for(int i = 0; i<legthOfArray; i++){
		if(keywordsArray[i]==str)
			return true;
	}
	return false;
}

/*
Function that takes in a character as a parameter and returns a boolean whether or not the character is an operator.
*/
bool checkIfSingleOperator(char c){
	int legthOfArray = 20;
	char operatorsArray[] = {'=', '<', '>', '+', '-', '*', '/', '%',
						'&', '!', '.', '(', ')', '[', ']', '{',
						'}', ';', ':', ','}; 
	
	for(int i = 0; i<=legthOfArray; i++){
		if(operatorsArray[i]==c)
			return true;
	}
	return false;
}

/*
Function that takes in a string as a parameter and returns a boolean whether or not the string is an operator.
*/
bool checkIfDoubleOperator(string str){
	int legthOfArray = 9;
	string operatorsArray[] = {"||", "&&", "==", "!=", "<=",
							">=", "++", "--", "->"}; 
	
	for(int i = 0; i<=legthOfArray; i++){
		if(operatorsArray[i]==str)
			return true;
	}
	return false;
}

