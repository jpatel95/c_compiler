/*
 * File:	lexer.cpp
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the lexical analyzer for Simple C.
 */

# include <cstdio>
# include <cctype>
# include <string>
# include <iostream>
# include "lexer.h"

using namespace std;
int numerrors, lineno = 1;



/* Later, we will associate token values with each keyword */

static string keywords[] = {
    "auto",
    "break",
    "case",
    "char",
    "const",
    "continue",
    "default",
    "do",
    "double",
    "else",
    "enum",
    "extern",
    "float",
    "for",
    "goto",
    "if",
    "int",
    "long",
    "register",
    "return",
    "short",
    "signed",
    "sizeof",
    "static",
    "struct",
    "switch",
    "typedef",
    "union",
    "unsigned",
    "void",
    "volatile",
    "while",
};

# define numKeywords (sizeof(keywords) / sizeof(keywords[0]))



/*
 * Function:	report
 *
 * Description:	Report an error to the standard error prefixed with the
 *		line number.  We'll be using this a lot later with an
 *		optional string argument, but C++'s stupid streams don't do
 *		positional arguments, so we actually resort to snprintf.
 *		You just can't beat C for doing things down and dirty.
 */

void report(const string &str, const string &arg)
{
    char buf[1000];

    snprintf(buf, sizeof(buf), str.c_str(), arg.c_str());
    cerr << "line " << lineno << ": " << buf << endl;
    numerrors ++;
}


/*
 * Function:	lexan
 *
 * Description:	Read and tokenize the standard input stream.  The lexeme is
 *		stored in a buffer.
 */

Token lexan()
{
    int p;
    unsigned i;
    static int c = cin.get();
    string lexbuf;

    /* The invariant here is that the next character has already been read
       and is ready to be classified.  In this way, we eliminate having to
       push back characters onto the stream, merely to read them again. */

    while (!cin.eof()) {
		lexbuf.clear();
		/* Ignore white space */
		while (isspace(c)) {
		    if (c == '\n')
				lineno ++;
			    c = cin.get();
		}
		/* Check for an identifier or a keyword */
		if (isalpha(c) || c == '_') {
		    do {
				lexbuf += c;
				c = cin.get();
		    } while (isalnum(c) || c == '_');

		    for (i = 0; i < numKeywords; i ++)
				if (keywords[i] == lexbuf)
				    break;
			if (i < numKeywords){
				//cout << "keyword:" << lexbuf << endl;
				if(lexbuf=="int"){
					return INT;
				}
				else if(lexbuf=="char"){
					return CHAR;
				}
				else if(lexbuf=="struct"){
					return STRUCT;
				}
				else if(lexbuf=="void"){
					return VOID;
				}
				else if(lexbuf=="else"){
					return ELSE;
				}
				else if(lexbuf=="return"){
					return RETURN;
				}
				else if(lexbuf=="while"){
					return WHILE;
				}
				else if(lexbuf=="sizeof"){
					return SIZEOF;
				}
				else{
					return JUNK;
				}
			}
			/*
			if (i < numKeywords){
				//cout << "keyword:" << lexbuf << endl;
		    }
		    */
		    else{
				//cout << "identifier:" << lexbuf << endl;
				return ID;
		    }
		    


		/* Check for a number */

		} else if (isdigit(c)) {
		    do {
				lexbuf += c;
				c = cin.get();
		    } while (isdigit(c));

		    //cout << "number:" << lexbuf << endl;
		    return NUM;


		/* There must be an easier way to do this.  It might seem stupid at
		   this point to recognize each token separately, but eventually
		   we'll be returning separate token values to the parser, so we
		   might as well do it now. */

		} else {
		    lexbuf += c;

		    switch(c) {
			    /* Check for '||' */

			    case '|':
					c = cin.get();

					if (c == '|') {
					    lexbuf += c;
					    c = cin.get();
					    //cout << "operator:" << lexbuf << endl;
					    //lookAhead = OR;
					    //matchEXPRESSION();
					    return OR;
					}

					cout << "illegal:" << lexbuf << endl;

			    /* Check for '=' and '==' */

			    case '=':
					c = cin.get();

					if (c == '=') {
					    lexbuf += c;
					    c = cin.get();
					    //lookAhead = EQLEQL;
					    //matchEQUALITY();
					    return EQLEQL;
					}

					//cout << "operator:" << lexbuf << endl;
					return EQL;


			    /* Check for '&' and '&&' */

			    case '&':
					c = cin.get();

					if (c == '&') {
					    lexbuf += c;
					    c = cin.get();
					    //lookAhead = AND;
					    //matchAND();
					    return AND;
					}

					//cout << "operator:" << lexbuf << endl;
					return ADDR;


			    /* Check for '!' and '!=' */

			    case '!':
					c = cin.get();

					if (c == '=') {
					    lexbuf += c;
					    c = cin.get();
					    //lookAhead = NEQ;
					    //matchEQUALITY();
					    return NEQ;
					}

					//cout << "operator:" << lexbuf << endl;
					return NOT;


			    /* Check for '<' and '<=' */

			    case '<':
					c = cin.get();

					if (c == '=') {
					    lexbuf += c;
					    c = cin.get();
					    //lookAhead = LEQ;
					    //matchINEQUALITY();
					    return LEQ;
					}

					//cout << "operator:" << lexbuf << endl;
					//lookAhead = LTN;
					//matchINEQUALITY();
					return LTN;


			    /* Check for '>' and '>=' */

			    case '>':
					c = cin.get();

					if (c == '=') {
					    lexbuf += c;
					    c = cin.get();
					    //lookAhead = GEQ;
					    //matchINEQUALITY();
					    return GEQ;
					}

					//cout << "operator:" << lexbuf << endl;
					//lookAhead = GTN;
					//matchINEQUALITY();
					return GTN;


			    /* Check for '-', '--', and '->' */

			    case '-':
					c = cin.get();

					if (c == '-') {
					    lexbuf += c;
					    c = cin.get();
					    return DECR;
					} else if (c == '>') {
					    lexbuf += c;
					    c = cin.get();
					    //******** NEED TO ADD
					    //lookAhead = EQLEQL;
					    //lookAhead=ARROW;
					    //matchINDEX_DOT_ARROW();
					    return ARROW;
					}

					//cout << "operator:" << lexbuf << endl;
					//lookAhead = SUB;
					//matchADD_SUB();
					return SUB;


			    /* Check for '+' and '++' */

			    case '+':
					c = cin.get();

					if (c == '+') {
					    lexbuf += c;
					    c = cin.get();
					    return INCR;
					}

					//cout << "operator:" << lexbuf << endl;
					//lookAhead = ADD;
					//matchADD_SUB();
					return ADD;


			    /* Check for simple, single character tokens */

			    case '*':
			    	//lookAhead = MUL;
			    	c = cin.get();
			    	//matchMUL_DIV_REM();
			    	return (Token)lexbuf[0];
			    case '%':
			    	//lookAhead = REM;
			    	c = cin.get();
			    	//matchMUL_DIV_REM();
			    	return (Token)lexbuf[0];
			    case ':':
			    	c = cin.get();
					//cout << "operator:" << lexbuf << endl;
					return (Token)lexbuf[0];
			    case ';':
			    	c = cin.get();
					//cout << "operator:" << lexbuf << endl;
					return (Token)lexbuf[0];
			    case '(':
			    	c = cin.get();
			    	//lookAhead = LEFTPAREN;
			    	//matchCAST();
					//cout << "operator:" << lexbuf << endl;
					return (Token)lexbuf[0];
			    case ')':
					c = cin.get();
					//cout << "operator:" << lexbuf << endl;
					return (Token)lexbuf[0];
				case '[':
					c = cin.get();
					//cout << "operator:" << lexbuf << endl;
					//lookAhead = LEFTBRACKET;
			    	//matchINDEX_DOT_ARROW();
					return (Token)lexbuf[0];
				case ']':
					c = cin.get();
					//cout << "operator:" << lexbuf << endl;
					return (Token)lexbuf[0];
			    case '{':
			    	c = cin.get();
					//cout << "operator:" << lexbuf << endl;
					return (Token)lexbuf[0];
			    case '}':
			    	c = cin.get();
					//cout << "operator:" << lexbuf << endl;
					return (Token)lexbuf[0];
			    case '.':
			    	c = cin.get();
					//cout << "operator:" << lexbuf << endl;
					//lookAhead = DOT;
			    	//matchINDEX_DOT_ARROW();
					return (Token)lexbuf[0];
			    case ',':
					c = cin.get();
					//cout << "operator:" << lexbuf << endl;
					return (Token)lexbuf[0];


			    /* Check for '/' or a comment */

			    case '/':
					c = cin.get();

					if (c == '*') {
					    do {
						while (c != '*' && !cin.eof()) {
						    if (c == '\n')
							lineno ++;

						    c = cin.get();
						}

						c = cin.get();
					    } while (c != '/' && !cin.eof());

					    c = cin.get();
					    break;

					} else {
					    //cout << "operator:" << lexbuf << endl;
					    //lookAhead = DIV;
			    		//matchMUL_DIV_REM();
					    return DIV;
					}


				/* Check for a string literal */

			    case '"':
					do {
					    p = c;
					    c = cin.get();
					    lexbuf += c;
					} while ((c != '"' || p == '\\') && c != '\n' && !cin.eof());

					if (c == '\n' || cin.eof())
					    report("malformed string literal");

					c = cin.get();
					//cout << "string:" << lexbuf << endl;
					//lookAhead = STRING;
					return STRING;


			    /* Check for a character literal */

			    case '\'':
					do {
					    p = c;
					    c = cin.get();
					    lexbuf += c;
					} while ((c != '\'' || p == '\\') && c != '\n' && !cin.eof());

					if (c == '\n' || cin.eof() || lexbuf.size() == 2)
					    report("malformed character literal");

					c = cin.get();
					//cout << "character:" << lexbuf << endl;
					//lookAhead = CHARACTER;
					return CHARACTER;


			    /* Handle EOF here as well */

			    case EOF:
			    	//lookAhead=ERROR;
					return ERROR;


			    /* Everything else is illegal */
			    default:
					c = cin.get();
					break;
			}
		}
    }
    //lookAhead=ERROR;
    return ERROR;
}


