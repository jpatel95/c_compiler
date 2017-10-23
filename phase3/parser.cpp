/*
 * File:	parser.c
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the recursive-descent parser for
 *		Simple C.
 */

# include <cstdlib>
# include <iostream>
# include <string>
# include <cstdlib>
# include "tokens.h"
# include "lexer.h"
# include "scope.h"
# include "checker.h"

using namespace std;

static int lookahead, nexttoken;
static string lexbuf, nextbuf;

static void expression();
static void statement();


/*
 * Function:	error
 *
 * Description:	Report a syntax error to standard error.
 */

static void error()
{
    if (lookahead == DONE)
		report("syntax error at end of file");
    else
		report("syntax error at '%s'", lexbuf);

    exit(EXIT_FAILURE);
}


/*
 * Function:	match
 *
 * Description:	Match the next token against the specified token.  A
 *		failure indicates a syntax error and will terminate the
 *		program since our parser does not do error recovery.
 */

static void match(int t)
{
    if (lookahead != t)
		error();

    if (nexttoken) {
		lookahead = nexttoken;
		lexbuf = nextbuf;
		nexttoken = 0;
    } else
		lookahead = lexan(lexbuf);
}


/*
 * Function:	peek
 *
 * Description:	Return the next token in the input stream and save it so
 *		that match() will later return it.
 */

static int peek()
{
    if (!nexttoken)
		nexttoken = lexan(nextbuf);

    return nexttoken;
}


/*
 * Function:	isSpecifier
 *
 * Description:	Return whether the given token is a type specifier.
 */

static bool isSpecifier(int token)
{
    return token == INT || token == CHAR || token == STRUCT;
}


/*
 * Function:	specifier
 *
 * Description:	Parse a type specifier.  Simple C has only ints, chars, and
 *		structure types.
 *
 *		specifier:
 *		  int
 *		  char
 *		  struct identifier
 */

static string specifier()
{
    if (lookahead == INT) {
		match(INT);
		return "int";
    }

    if (lookahead == CHAR) {
		match(CHAR);
		return "char";
    }
	match(STRUCT);
	string s = lexbuf;
	match(ID);
    return s;
}


/*
 * Function:	pointers
 *
 * Description:	Parse pointer declarators (i.e., zero or more asterisks).
 *
 *		pointers:
 *		  empty
 *		  * pointers
 */

static int pointers()
{
	int count = 0;
    while (lookahead == '*'){
    	match('*');
    	count++;
    }
    return count;
}


/*
 * Function:	declarator
 *
 * Description:	Parse a declarator, which in Simple C is either a scalar
 *		variable or an array, with optional pointer declarators.
 *
 *		declarator:
 *		  pointers identifier
 *		  pointers identifier [ num ]
 */

static void declarator(string str)
{
    int pointerCount = pointers();
    string idStr = lexbuf;
    string length;
    match(ID);

    if(pointerCount>0){
    	cout<<"Declaring "<< idStr << " as a "  << str << " pointer "<<endl;
    }
    else {
    	cout<<"Declaring "<< idStr << " as a "  << str <<endl; 
    }
    if (lookahead == '[') {
		match('[');
		length=lexbuf;
		match(NUM);
		match(']');
		declareVariable(idStr, str, pointerCount, ARRAY, atoi(length.c_str()), false);

    }
    else if(str!="int" && str!="char"){
    	declareStruct(idStr, str, pointerCount, SCALAR, -1, true);
    }
    else{
    	declareVariable(idStr, str, pointerCount, SCALAR, 0, false);
    }
}


/*
 * Function:	declaration
 *
 * Description:	Parse a local variable declaration.  Global declarations
 *		are handled separately since we need to detect a function
 *		as a special case.
 *
 *		declaration:
 *		  specifier declarator-list ';'
 *
 *		declarator-list:
 *		  declarator
 *		  declarator , declarator-list
 */

static void declaration()
{
    string s = specifier();
    declarator(s);

    while (lookahead == ',') {
		match(',');
		declarator(s);
    }

    match(';');
}


/*
 * Function:	declarations
 *
 * Description:	Parse a possibly empty sequence of declarations.
 *
 *		declarations:
 *		  empty
 *		  declaration declarations
 */

static void declarations()
{
    while (isSpecifier(lookahead))
		declaration();
}


/*
 * Function:	primaryExpression
 *
 * Description:	Parse a primary expression.
 *
 *		primary-expression:
 *		  ( expression )
 *		  identifier ( expression-list )
 *		  identifier ( )
 *		  identifier
 *		  character
 *		  string
 *		  num
 *
 *		expression-list:
 *		  expression
 *		  expression , expression-list
 */

static void primaryExpression()
{
    if (lookahead == '(') {
		match('(');
		expression();
		match(')');
    } else if (lookahead == CHARACTER) {
		match(CHARACTER);
    } else if (lookahead == STRING) {
		match(STRING);
    } else if (lookahead == NUM) {
		match(NUM);
    } else if (lookahead == ID) {
    	string idStr = lexbuf;
    	checkIfExists(idStr);
		match(ID);
		if (lookahead == '(') {
		    match('(');
		    if (lookahead != ')') {
				expression();
				while (lookahead == ',') {
				    match(',');
				    expression();
				}
			}
		    match(')');
		}
    } else{
		error();
	}
}


/*
 * Function:	postfixExpression
 *
 * Description:	Parse a postfix expression.
 *
 *		postfix-expression:
 *		  primary-expression
 *		  postfix-expression [ expression ]
 *		  postfix-expression . identifier
 *		  postfix-expression -> identifier
 */

static void postfixExpression()
{
    primaryExpression();

    while (1) {
	if (lookahead == '[') {
	    match('[');
	    expression();
	    match(']');
	    cout << "index" << endl;

	} else if (lookahead == '.') {
	    match('.');
	    match(ID);
	    cout << "dot" << endl;

	} else if (lookahead == ARROW) {
	    match(ARROW);
	    match(ID);
	    cout << "arrow" << endl;

	} else
	    break;
    }
}


/*
 * Function:	prefixExpression
 *
 * Description:	Parse a prefix expression.
 *
 *		prefix-expression:
 *		  postfix-expression
 *		  ! prefix-expression
 *		  - prefix-expression
 *		  * prefix-expression
 *		  & prefix-expression
 *		  sizeof prefix-expression
 *		  sizeof ( specifier pointers )
 */

static void prefixExpression()
{
    if (lookahead == '!') {
		match('!');
		prefixExpression();
		cout << "not" << endl;
    } else if (lookahead == '-') {
		match('-');
		prefixExpression();
		cout << "neg" << endl;
    } else if (lookahead == '*') {
		match('*');
		prefixExpression();
		cout << "deref" << endl;
    } else if (lookahead == '&') {
		match('&');
		prefixExpression();
		cout << "addr" << endl;
    } else if (lookahead == SIZEOF) {
		match(SIZEOF);
		if (lookahead == '(' && isSpecifier(peek())) {
		    match('(');
		    specifier();
		    pointers();
		    match(')');
		} else
		    prefixExpression();
		cout << "sizeof" << endl;
    } else
		postfixExpression();
}


/*
 * Function:	castExpression
 *
 * Description:	Parse a cast expression.  If the token after the opening
 *		parenthesis is not a type specifier, we could have a
 *		parenthesized expression instead.
 *
 *		cast-expression:
 *		  prefix-expression
 *		  ( specifier pointers ) cast-expression
 */

static void castExpression()
{
    if (lookahead == '(' && isSpecifier(peek())) {
		match('(');
		specifier();
		pointers();
		match(')');
		castExpression();
		cout << "cast" << endl;
    } else
		prefixExpression();
}


/*
 * Function:	multiplicativeExpression
 *
 * Description:	Parse a multiplicative expression.
 *
 *		multiplicative-expression:
 *		  cast-expression
 *		  multiplicative-expression * cast-expression
 *		  multiplicative-expression / cast-expression
 *		  multiplicative-expression % cast-expression
 */

static void multiplicativeExpression()
{
    castExpression();

    while (1) {
	if (lookahead == '*') {
	    match('*');
	    castExpression();
	    cout << "mul" << endl;

	} else if (lookahead == '/') {
	    match('/');
	    castExpression();
	    cout << "div" << endl;

	} else if (lookahead == '%') {
	    match('%');
	    castExpression();
	    cout << "rem" << endl;

	} else
	    break;
    }
}


/*
 * Function:	additiveExpression
 *
 * Description:	Parse an additive expression.
 *
 *		additive-expression:
 *		  multiplicative-expression
 *		  additive-expression + multiplicative-expression
 *		  additive-expression - multiplicative-expression
 */

static void additiveExpression()
{
    multiplicativeExpression();

    while (1) {
		if (lookahead == '+') {
		    match('+');
		    multiplicativeExpression();
		    cout << "add" << endl;

		} else if (lookahead == '-') {
		    match('-');
		    multiplicativeExpression();
		    cout << "sub" << endl;

		} else{
		    break;
		}
    }
}


/*
 * Function:	relationalExpression
 *
 * Description:	Parse a relational expression.  Note that Simple C does not
 *		have shift operators, so we go immediately to additive
 *		expressions.
 *
 *		relational-expression:
 *		  additive-expression
 *		  relational-expression < additive-expression
 *		  relational-expression > additive-expression
 *		  relational-expression <= additive-expression
 *		  relational-expression >= additive-expression
 */

static void relationalExpression()
{
    additiveExpression();

    while (1) {
		if (lookahead == '<') {
		    match('<');
		    additiveExpression();
		    cout << "ltn" << endl;

		} else if (lookahead == '>') {
		    match('>');
		    additiveExpression();
		    cout << "gtn" << endl;

		} else if (lookahead == LEQ) {
		    match(LEQ);
		    additiveExpression();
		    cout << "leq" << endl;

		} else if (lookahead == GEQ) {
		    match(GEQ);
		    additiveExpression();
		    cout << "geq" << endl;

		} else {
		    break;
		}
	}
}


/*
 * Function:	equalityExpression
 *
 * Description:	Parse an equality expression.
 *
 *		equality-expression:
 *		  relational-expression
 *		  equality-expression == relational-expression
 *		  equality-expression != relational-expression
 */

static void equalityExpression()
{
    relationalExpression();

    while (1) {
		if (lookahead == EQL) {
		    match(EQL);
		    relationalExpression();
		    cout << "eql" << endl;

		} else if (lookahead == NEQ) {
		    match(NEQ);
		    relationalExpression();
		    cout << "neq" << endl;

		} else{
		    break;
	    }
	}
}


/*
 * Function:	logicalAndExpression
 *
 * Description:	Parse a logical-and expression.  Note that Simple C does
 *		not have bitwise-and expressions.
 *
 *		logical-and-expression:
 *		  equality-expression
 *		  logical-and-expression && equality-expression
 */

static void logicalAndExpression()
{
    equalityExpression();

    while (lookahead == AND) {
		match(AND);
		equalityExpression();
		cout << "and" << endl;
    }
}


/*
 * Function:	expression
 *
 * Description:	Parse an expression, or more specifically, a logical-or
 *		expression, since Simple C does not allow comma or
 *		assignment as an expression operator.
 *
 *		expression:
 *		  logical-and-expression
 *		  expression || logical-and-expression
 */

static void expression()
{
    logicalAndExpression();

    while (lookahead == OR) {
		match(OR);
		logicalAndExpression();
		cout << "or" << endl;
    }
}


/*
 * Function:	statements
 *
 * Description:	Parse a possibly empty sequence of statements.  Rather than
 *		checking if the next token starts a statement, we check if
 *		the next token ends the sequence, since a sequence of
 *		statements is always terminated by a closing brace.
 *
 *		statements:
 *		  empty
 *		  statement statements
 */

static void statements()
{
    while (lookahead != '}')
		statement();
}


/*
 * Function:	statement
 *
 * Description:	Parse a statement.  Note that Simple C has so few
 *		statements that we handle them all in this one function.
 *
 *		statement:
 *		  { declarations statements }
 *		  return expression ;
 *		  while ( expression ) statement
 *		  if ( expression ) statement
 *		  if ( expression ) statement else statement
 *		  expression = expression ;
 *		  expression ;
 */

static void statement()
{
    if (lookahead == '{') {
		match('{');
		cout<<"Open Block Scope"<<endl;
		openScope();
		declarations();
		statements();
		cout<<"Close Block Scope"<<endl;
		match('}');
		closeScope();

    } else if (lookahead == RETURN) {
		match(RETURN);
		expression();
		match(';');
    } else if (lookahead == WHILE) {
		match(WHILE);
		match('(');
		expression();
		match(')');
		statement();
    } else if (lookahead == IF) {
		match(IF);
		match('(');
		expression();
		match(')');
		statement();
		if (lookahead == ELSE) {
		    match(ELSE);
		    statement();
		}
    } else {
		expression();
		if (lookahead == '=') {
		    match('=');
		    expression();
		}
	match(';');
    }
}


/*
 * Function:	parameter
 *
 * Description:	Parse a parameter, which in Simple C is always a scalar
 *		variable with optional pointer declarators.
 *
 *		parameter:
 *		  specifier pointers identifier
 */

static void parameter()
{
	string specifierStr = lexbuf;
	string idStr;
	int pointerCount;
    specifier();
    pointerCount=pointers();
    idStr = lexbuf;
    match(ID);
    if(specifierStr=="struct"){
    	declareStruct(idStr, specifierStr, pointerCount, SCALAR, 0, true);
    }else{
    	declareVariable(idStr, specifierStr, pointerCount, SCALAR, 0, false);
    }
}


/*
 * Function:	parameters
 *
 * Description:	Parse the parameters of a function, but not the opening or
 *		closing parentheses.
 *
 *		parameters:
 *		  void
 *		  parameter-list
 *
 *		parameter-list:
 *		  parameter
 *		  parameter , parameter-list
 */

static void parameters()
{
    if (lookahead == VOID)
		match(VOID);
    else {
		parameter();

		while (lookahead == ',') {
		    match(',');
		    parameter();
		}
    }
}


/*
 * Function:	topLevelDeclaration
 *
 * Description:	Parse a top level declaration, which is either a structure
 *		type definition, function definition, or global variable
 *		declaration.
 *
 *		type-definition:
 *		  struct identifier { declaration declarations } ;
 *
 *		global-declaration:
 *		  specifier global-declarator-list ;
 *
 *		global-declarator-list:
 *		  global-declarator
 *		  global-declarator , global-declarator-list
 *
 *		global-declarator:
 *		  pointers identifier
 *		  pointers identifier ( )
 *		  pointers identifier [ num ]
 *
 *		function-definition:
 *		  specifier pointers identifier ( parameters ) { ... }
 */

static void topLevelDeclaration()
{
	bool isStruct = false;
	if(lexbuf == "struct"){
		isStruct = true;
	}
	string specifierStr = specifier();
	string idStr;
	string arraySize;
	int pointerCount;
    if ( isStruct && lookahead == '{') {
    	cout<<"Defining stuct " << specifierStr <<endl;
		match('{');
		openScope();
		declaration();
		declarations();
		match('}');
		defineStruct("", specifierStr, 0, SCALAR, -2, true);
		closeScope();
		match(';');
    }
    else {
		pointerCount = pointers();
		idStr = lexbuf;
		match(ID);
		if (lookahead == '[') { //HANDLES ARRAYS and Array of pointers
		    match('[');
		    arraySize = lexbuf;
		    match(NUM);
		    match(']');
		    if(pointerCount>0){
		    	cout<<"Declaring "<< idStr << " as an "  << specifierStr << " pointer's array of size "<<arraySize<<endl;
		    }
		    else {
		    	cout<<"Declaring "<< idStr << " as an "  << specifierStr << " array of size "<<arraySize<<endl;
		    }
		    declareVariable(idStr, specifierStr, pointerCount, ARRAY, atoi(arraySize.c_str()), false);
		} else if (lookahead == '(') {
		    match('(');
		    if (lookahead == ')'){ //FUNCTION DECLARATION
				match(')');
				if(isStruct){
					if(pointerCount>0){
			    		cout<<"Declaring "<< idStr << " as a function declaration returning a struct "  << specifierStr << " pointer "<<endl;
				    }
				    else {
				    	cout<<"Declaring "<< idStr << " as a function declaration returning a struct "  << specifierStr <<endl;
				    }
				    declareFunction(idStr, specifierStr, pointerCount, FUNCTION, -1, true);
				}
				else{
					if(pointerCount>0){
			    		cout<<"Declaring "<< idStr << " as a function declaration returning a "  << specifierStr << " pointer "<<endl;
				    }
				    else {
				    	cout<<"Declaring "<< idStr << " as a function declaration returning a "  << specifierStr <<endl;
				    }
				   	declareFunction(idStr, specifierStr, pointerCount, FUNCTION, -1, false);

				}

			}
		    else { //FUNCTION DEFINITION
		    	openScope();
		    	if(isStruct){
		    		if(pointerCount>0){
						cout<<"Open struct "<<specifierStr<<" pointer "<<idStr<<" function scope"<<endl;
				    }
				    else {
						cout<<"Open struct "<<specifierStr<<" "<<idStr<<" function scope"<<endl;
				    }
				    defineFunction(idStr, specifierStr, pointerCount, FUNCTION, -2, true);
		    	}
		    	else{
		    		if(pointerCount>0){
						cout<<"Open "<<specifierStr<<" pointer "<<idStr<<" function scope"<<endl;
				    }
				    else {
						cout<<"Open "<<specifierStr<<" "<<idStr<<" function scope"<<endl;
				    }
				    defineFunction(idStr, specifierStr, pointerCount, FUNCTION, -2, false);
		    	}

				parameters();
				match(')');
				match('{');
				declarations();
				statements();
				match('}');
				closeScope();

				if(isStruct){
					if(pointerCount>0){
						cout<<"Close struct "<<specifierStr<<" pointer "<<idStr<<" function scope"<<endl;
				    }
				    else {
						cout<<"Close struct "<<specifierStr<<" "<<idStr<<" function scope"<<endl;
				    }
				}
				else{
					if(pointerCount>0){
					cout<<"Close "<<specifierStr<<" pointer "<<idStr<<" function scope"<<endl;
				    }
				    else {
						cout<<"Close "<<specifierStr<<" "<<idStr<<" function scope"<<endl;
				    }
				}
				
			    return;
		    }
		} else { //Else its  global variable so you just want to print the values
			if(isStruct){
				if(pointerCount>0){
		    		cout<<"Declaring struct "<< specifierStr <<" "<< idStr << " as an pointer "<<endl;
			    }
			    else {
			    	cout<<"Declaring struct "<< specifierStr <<" " <<idStr <<endl;
			    }
			   	declareStruct(idStr, specifierStr, pointerCount, SCALAR, -1, true);
			}
			else{
				if(pointerCount>0){
		    		cout<<"Declaring "<< idStr << " as an "  << specifierStr << " pointer "<<endl;
			    }
			    else {
			    	cout<<"Declaring "<< idStr << " as an "  << specifierStr <<endl;
			    }
			    declareVariable(idStr, specifierStr, pointerCount, SCALAR, 0, false);
			}
		}

		while (lookahead == ',') {
		    match(',');
		    pointerCount=pointers();
		    idStr=lexbuf;
		    match(ID);
		    if (lookahead == '[') {
				match('[');
				string len = lexbuf;
				match(NUM);
				match(']');
				if(pointerCount>0){
		    		cout<<"Declaring "<< idStr << " as an "  << specifierStr << " pointer's array of size "<<arraySize<<endl;
			    }
			    else {
			    	cout<<"Declaring "<< idStr << " as an "  << specifierStr << " array of size "<<arraySize<<endl;
			    }
			    declareVariable(idStr, specifierStr, pointerCount, ARRAY, atoi(len.c_str()), false);
		    } else if (lookahead == '(') {
				match('(');
				match(')');
				if(pointerCount>0){
			    	cout<<"Declaring "<< idStr << " as a function definition returning a "  << specifierStr << " pointer "<<endl;
			    }
			    else {
			    	cout<<"Declaring "<< idStr << " as a function definition returning a "  << specifierStr <<endl;
			    }
			    declareFunction(idStr, specifierStr, pointerCount, FUNCTION, -1, false);
		    }
		    else{
		    	if(isStruct){
					if(pointerCount>0){
			    		cout<<"Declaring struct "<< specifierStr <<" "<< idStr << " as an pointer "<<endl;
				    }
				    else {
				    	cout<<"Declaring struct "<< specifierStr <<" " <<idStr <<endl;
				    }
				    declareStruct(idStr, specifierStr, pointerCount, SCALAR, -1, true);
				}
				else{
					declareVariable(idStr, specifierStr, pointerCount, SCALAR, 0, false);
				}
		    }
		}
		match(';');
    }
}


/*
 * Function:	main
 *
 * Description:	Analyze the standard input stream.
 */

int main()
{
    lookahead = lexan(lexbuf);
    cout<<"Open File"<<endl;
    openScope();
    cout<<"File Opened"<<endl;

    while (lookahead != DONE)
		topLevelDeclaration();

	cout<<"Close File"<<endl;
	closeScope();
	cout<<"File Closed"<<endl;

    exit(EXIT_SUCCESS);
}
