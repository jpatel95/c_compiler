# include <cstdio>
# include <cctype>
# include <string>
# include <iostream>
# include "lexer.h"

using namespace std;

int getNext();
void matchEXPRESSION();
void matchAND();
void matchEQUALITY();
void matchINEQUALITY();
void matchADD_SUB();
void matchMUL_DIV_REM();
void matchCAST();
void matchUNARY();
void matchINDEX_DOT_ARROW();
void matchSPECIFIER();
void matchPOINTERS();
void otherExpression();
void top_level();
void translation_unit();
void parameters();
void parameter_list();
void parameter();
void declarations();
void declaration();
void declarator_list();
void declarator();
void statements();
void statement();
Token lexan();

/*
Matching Functions
*/
int lookAhead;
int peekNextToken;


bool checkIfSpecifier(int t){
	//cout<<"checkIfSpecifier: "<<lookAhead<<" "<<peekNextToken<<" "<<t<<" "<<INT<<" "<<CHAR<<" "<<STRUCT<<endl;
	if(t==INT || t==CHAR || t==STRUCT)
		return true;
	return false;
}


int getNext(){
	if(peekNextToken==0){
		peekNextToken=lexan();
	}
	return peekNextToken;
}

void match(int t){
	if(peekNextToken>0){
		lookAhead=peekNextToken;
		peekNextToken=0;
	}
	else{
		lookAhead=lexan();
	}
}

void expressionList(){
	matchEXPRESSION();
	while(lookAhead==COMMA){
		match(COMMA);
		matchEXPRESSION();
	}
}

void otherExpression(){
	//cout<<"Inside other"<<endl;
	//cout<<lookAhead<<" "<<peekNextToken<<" "<<endl;
	if(lookAhead==LEFTPAREN){
		match(LEFTPAREN);
		matchEXPRESSION();
		match(RIGHTPAREN);
	}
	else if(lookAhead==NUM || lookAhead==STRING ||lookAhead==CHAR){
		match(lookAhead);
	}
	else if(lookAhead==ID){
		match(lookAhead);
		if(lookAhead==LEFTPAREN){
			match(lookAhead);
			if(lookAhead!=RIGHTPAREN){
				expressionList();
			}
			match(RIGHTPAREN);
		}
	}
}


void matchINDEX_DOT_ARROW(){
	//cout<<"Inside matchINDEX_DOT_ARROW"<<endl;
	//cout<<lookAhead<<" "<<peekNextToken<<" "<<endl;
	otherExpression();
	while(lookAhead==LEFTBRACKET || lookAhead == DOT || lookAhead==ARROW){
		int prev = lookAhead;
		match(lookAhead);
		matchEXPRESSION();

		if(prev==LEFTBRACKET){
			match(RIGHTBRACKET);
			cout<<"index"<<endl;
		}
		else if(prev==DOT){
			cout<<"dot"<<endl;
		}
		else{
			cout<<"arrow"<<endl;
		}
	}
}

void matchUNARY(){
	//cout<<"Inside matchUNARY"<<endl;
	//cout<<lookAhead<<" "<<peekNextToken<<" "<<endl;
	if(lookAhead==ADDR){
		match(lookAhead);
		matchUNARY();
		cout<<"addr"<<endl;
	}
	else if(lookAhead==MUL){
		match(lookAhead);
		matchUNARY();
		cout<<"deref"<<endl;
	}
	else if(lookAhead==NOT){
		match(lookAhead);
		matchUNARY();
		cout<<"not"<<endl;
	}
	else if(lookAhead==SUB){
		match(lookAhead);
		matchUNARY();
		cout<<"neg"<<endl;
	}
	else if(lookAhead==SIZEOF){
		match(lookAhead);
		if(lookAhead==LEFTPAREN){
			match(LEFTPAREN);
			matchSPECIFIER();
			matchPOINTERS();
			match(RIGHTPAREN);
		}
		else{
			matchUNARY();
		}
		cout<<"sizeof"<<endl;

	}
	else{
		matchINDEX_DOT_ARROW();
	}
}

void matchPOINTERS(){
	while(lookAhead=='*'){
		match('*');
	}
}

void matchSPECIFIER(){
	if(lookAhead==INT){
		match(INT);
	}
	else if(lookAhead==CHAR){
		match(CHAR);
	}
	else{
		match(STRUCT);
		match(ID);
	}
}

void matchCAST(){
	//cout<<"Inside matchCAST"<<endl;
	//cout<<lookAhead<<" "<<peekNextToken<<" "<<LEFTPAREN<<" "<<INT<<endl;
	//cout<<lookAhead<<" "<<peekNextToken<<" "<<LEFTPAREN<<" "<<INT<<endl;

	if(lookAhead==LEFTPAREN && checkIfSpecifier(getNext())){
		match(LEFTPAREN);
		matchSPECIFIER();
		matchPOINTERS();
		match(RIGHTPAREN);
		matchCAST();
		cout<<"cast"<<endl;
	}
	else{
		matchUNARY();
	}
}

void matchMUL_DIV_REM(){
	//cout<<"Inside matchMUL_DIV_REM"<<endl;
	//cout<<lookAhead<<" "<<MUL<< " "<<DIV<<" "<<REM<<endl;
	matchCAST();
	while(lookAhead == MUL || lookAhead == DIV || lookAhead == REM){
		int prev = lookAhead;
		match(lookAhead);
		matchCAST();
		if(prev == MUL)
			cout<<"mul"<<endl;
		else if(prev == DIV)
			cout<<"div"<<endl;
		else
			cout<<"rem"<<endl;
	}
}

void matchADD_SUB(){
	matchMUL_DIV_REM();
	while(lookAhead == ADD || lookAhead == SUB){
		int prev = lookAhead;
		match(lookAhead);
		matchMUL_DIV_REM();
		if(prev == ADD)
			cout<<"add"<<endl;
		else
			cout<<"sub"<<endl;
	}
}

void matchINEQUALITY(){
	//cout<<"Inside matchINEQUALITY"<<endl;
	//cout<<lookAhead<<" "<<peekNextToken<<" "<<endl;
	matchADD_SUB();
	while(lookAhead == LTN || lookAhead == GTN || lookAhead == LEQ || lookAhead == GEQ){
		int prev = lookAhead;
		match(lookAhead);
		matchADD_SUB();
		if(prev == LTN){
			cout << "ltn" << endl;
		}
		else if(prev == GTN){
			cout << "gtn" << endl;
		}
		else if(prev == LEQ){
			cout << "leq" << endl;
		}
		else{
			cout << "geq" << endl;
		}
	}
}

void matchEQUALITY(){
	matchINEQUALITY();
	while(lookAhead == EQLEQL || lookAhead == NEQ){
		int prev = lookAhead;
		match(lookAhead);
		matchINEQUALITY();
		if(prev == EQLEQL)
			cout<<"eql"<<endl;
		else
			cout<<"neq"<<endl;
	}
}

void matchAND(){
	matchEQUALITY();
	while(lookAhead == AND){
		match(AND);
		matchEQUALITY();
		cout<<"and"<<endl;
	}
}

void matchEXPRESSION(){
	matchAND();
	while(lookAhead == OR){
		match(OR);
		matchAND();
		cout<<"or"<<endl;
	}
}

/** RULES **/
/** RULES **/
/** RULES **/

void statement(){
	//cout<<"HERE "<<lookAhead<<endl;
	if(lookAhead==LEFTBRACE){
		match(LEFTBRACE);
		declarations();
		statements();
		match(RIGHTBRACE);
	}
	else if(lookAhead==RETURN){
		match(RETURN);
		matchEXPRESSION();
		match(SEMICOLON);
	}
	else if(lookAhead==WHILE){
		match(WHILE);
		match(LEFTPAREN);
		matchEXPRESSION();
		match(RIGHTPAREN);
		statement();
	}
	else if(lookAhead==IF){
		match(IF);
		match(LEFTPAREN);
		matchEXPRESSION();
		match(RIGHTPAREN);
		statement();
		if(lookAhead==ELSE){
			match(ELSE);
			statement();
		}
	}
	else{
		matchEXPRESSION();
		if(lookAhead==EQL){
			match(EQL);
			matchEXPRESSION();
		}
		match(SEMICOLON);
	}
}

void statements(){
	while(lookAhead!=RIGHTBRACE){
		statement();
	}
}


void declarator(){
	matchPOINTERS();
	match(ID);
	if(lookAhead==LEFTBRACKET){
		match(LEFTBRACKET);
		match(NUM);
		match(RIGHTBRACKET);
	}
}

void declarator_list(){
	declarator();
	while(lookAhead==COMMA){
		match(COMMA);
		declarator();
	}
}

void declaration(){
	matchSPECIFIER();
	declarator_list();
	match(SEMICOLON);
}


void declarations(){
	while(checkIfSpecifier(lookAhead)){
		declaration();
	}
}


void parameter(){
	matchSPECIFIER();
	matchPOINTERS();
	match(ID);
}

void parameter_list(){
	parameter();
	while(lookAhead==COMMA){
		match(COMMA);
		parameter();
	}
}

void parameters(){
	if(lookAhead==VOID){
		match(VOID);
	}
	else{
		parameter_list();
	}
}


/*
This is the top level function that takes into consideration a type definition, function def, or global decl.
*/
void top_level(){
	bool isStruct = false;
	bool isID = false;
	if(lookAhead==STRUCT){
		isStruct = true;
	}
	matchSPECIFIER();
	if(lookAhead==ID){
		isID=true;
	}

	matchPOINTERS();
	match(ID);
	if(lookAhead==LEFTBRACE && isStruct && isID){ //then you know its a typedef
		match(LEFTBRACE);
		declaration();
		declarations();
		match(RIGHTBRACE);
		top_level();
	}
	else if(lookAhead==LEFTBRACKET){ // global declaration
		match(LEFTBRACKET);
		match(NUM);
		match(RIGHTBRACKET);
		top_level();
	}
	else if(lookAhead==LEFTPAREN){ // global declaration or function def
		match(LEFTPAREN);
		if(lookAhead==RIGHTPAREN){// so its a global declarator
			match(RIGHTPAREN);
		}
		else{ //else it was a function definiton
			parameters();
			match(RIGHTPAREN);
			match(LEFTBRACE);
			declarations();
			statements();
			match(RIGHTBRACE);
		}
		top_level();
	}
}



/*
 * Function:	main
 *
 * Description:	Read and tokenize and standard input stream.
 */
int main(){
	lookAhead=lexan();
    while (lookAhead>0){
    	//cout<<"Main: "<<lookAhead<<" "<<peekNextToken<<" "<<endl;
		top_level();
		lookAhead=lexan();
    }
    return 0;
}