/*
 * File:	checker.cpp
 *
 * Description:	This file contains the public and private function and
 *		variable definitions for the semantic checker for Simple C.
 *
 *		Extra functionality:
 *		- inserting an undeclared symbol with the error type
 */

# include <map>
# include <iostream>
# include "lexer.h"
# include "checker.h"
# include "nullptr.h"
# include "tokens.h"
# include "Symbol.h"
# include "Scope.h"
# include "Type.h"


using namespace std;

static map<string,Scope*> fields;
static Scope *outermost, *toplevel;
static const Type error;

static string undeclared = "'%s' undeclared";
static string redefined = "redefinition of '%s'";
static string redeclared = "redeclaration of '%s'";
static string conflicting = "conflicting types for '%s'";
static string incomplete = "incomplete type for '%s'";
static string nonpointer = "pointer type required for '%s'";

static string E1 = "invalid return type";
static string E2 = "invalid type for test expression";
static string E3 = "lvalue required in expression";
static string E4 = "invalid operands to binary %s";
static string E5 = "invalid operand to unary %s";
static string E6 = "invalid operand in cast expression";
static string E7 = "invalid operand in sizeof expression";
static string E8 = "called object is not a function";
static string E9 = "invalid arguments to called function";
static string E10 = "using pointer to incomplete type";



/*
 * Function:	checkIfComplete
 *
 * Description:	Check if the given type is complete.  A non-structure type
 *		is always complete.  A structure type is complete if its
 *		fields have been defined.
 */

static Type checkIfComplete(const string &name, const Type &type)
{
    if (!type.isStruct() || type.indirection() > 0)
	return type;

    if (fields.count(type.specifier()) > 0)
	return type;

    report(incomplete, name);
    return error;
}


/*
 * Function:	checkIfStructure
 *
 * Description:	Check if the given type is a structure.
 */

static Type checkIfStructure(const string &name, const Type &type)
{
    if (!type.isStruct() || type.indirection() > 0)
	return type;

    report(nonpointer, name);
    return error;
}




/*
 * Function:	openScope
 *
 * Description:	Create a scope and make it the new top-level scope.
 */

Scope *openScope()
{
    toplevel = new Scope(toplevel);

    if (outermost == nullptr)
	outermost = toplevel;

    return toplevel;
}


/*
 * Function:	closeScope
 *
 * Description:	Remove the top-level scope, and make its enclosing scope
 *		the new top-level scope.
 */

Scope *closeScope()
{
    Scope *old = toplevel;

    toplevel = toplevel->enclosing();
    return old;
}


/*
 * Function:	defineStructure
 *
 * Description:	Define a structure with the specified NAME and whose fields
 *		are specified by SCOPE.  A structure can be defined only
 *		once.
 */

void defineStructure(const string &name, Scope *scope)
{
    if (fields.count(name) > 0) {
	report(redefined, name);
	delete scope;
    } else
	fields[name] = scope;
}


/*
 * Function:	defineFunction
 *
 * Description:	Define a function with the specified NAME and TYPE.  A
 *		function is always defined in the outermost scope.  This
 *		definition always replaces any previous definition or
 *		declaration.
 */

Symbol *defineFunction(const string &name, const Type &type)
{
    Symbol *symbol = outermost->find(name);

    if (symbol != nullptr) {
	if (symbol->type().isFunction() && symbol->type().parameters()) {
	    report(redefined, name);
	    delete symbol->type().parameters();

	} else if (type != symbol->type())
	    report(conflicting, name);

	outermost->remove(name);
	delete symbol;
    }

    symbol = new Symbol(name, checkIfStructure(name, type));
    outermost->insert(symbol);

    return symbol;
}


/*
 * Function:	declareFunction
 *
 * Description:	Declare a function with the specified NAME and TYPE.  A
 *		function is always declared in the outermost scope.  Any
 *		redeclaration is discarded.
 */

Symbol *declareFunction(const string &name, const Type &type)
{
    Symbol *symbol = outermost->find(name);

    if (symbol == nullptr) {
	symbol = new Symbol(name, checkIfStructure(name, type));
	outermost->insert(symbol);

    } else if (type != symbol->type()) {
	report(conflicting, name);
	delete type.parameters();
    }

    return symbol;
}


/*
 * Function:	declareParameter
 *
 * Description:	Declare a parameter with the specified NAME and TYPE.  Any
 *		redeclaration is discarded.  The only difference between
 *		declaring a parameter and a variable is that a parameter
 *		cannot be a structure type.
 */

Symbol *declareParameter(const string &name, const Type &type)
{
    return declareVariable(name, checkIfStructure(name, type));
}


/*
 * Function:	declareVariable
 *
 * Description:	Declare a variable with the specified NAME and TYPE.  Any
 *		redeclaration is discarded.
 */

Symbol *declareVariable(const string &name, const Type &type)
{
    Symbol *symbol = toplevel->find(name);

    if (symbol == nullptr) {
	symbol = new Symbol(name, checkIfComplete(name, type));
	toplevel->insert(symbol);

    } else if (outermost != toplevel)
	report(redeclared, name);

    else if (type != symbol->type())
	report(conflicting, name);

    return symbol;
}


/*
 * Function:	checkIdentifier
 *
 * Description:	Check if NAME is declared.  If it is undeclared, then
 *		declare it as having the error type in order to eliminate
 *		future error messages.
 */

Symbol *checkIdentifier(const string &name)
{
    Symbol *symbol = toplevel->lookup(name);

    if (symbol == nullptr) {
    	report(undeclared, name);
    	symbol = new Symbol(name, error);
    	toplevel->insert(symbol);
    }

    return symbol;
}


bool isCompatible(const Type &left, const Type &right){
    Type l = left.promote();
    Type r = right.promote();

    if(l.specifier()!=right.specifier() || l.indirection()!=r.indirection()){
        return false;
    }
    return true;
}

bool checkComplete(const Type &t){
    if(t.isStruct() && t.indirection()>0 && fields.count(t.specifier()) == 0){
        return false;
    }
    return true;
}

void checkWhile(const Type &t){
    if(t==err){
        return;
    }
    if(!t.isSimple()){
        report(E2);
        return;
    }
}

void checkIf(const Type &t){
    if(t==err){
        return;
    }

    if(!t.isSimple()){
        report(E2);
        return;
    }
}

void checkAssignment(const Type &left, const Type &right, bool &lvalue){
    if(left==err || right==err){
        return;
    }
    if(!lvalue){
        report(E3);
        return;
    }
    if(left.promote().specifier()!=right.promote().specifier() || left.indirection()!=right.indirection()){
        cout<<left<<" ------ "<<right<<endl;
        report(E4, "=");
        return;
    }
}

void checkReturn(const Type &left, const Type &right){
    if(left==err || right == err){
        return;
    }
    //cout<<"Left: "<< left<<" Right: "<<right<<endl;
    if(left.specifier()!=right.specifier() || left.promote().indirection()!=right.indirection()){
        report(E1);
        return;
    }

}



Type checkLogicalExpression(const Type &left, const Type &right, const string &op){
    if(left == err || right == err){
        return err;
    }
    if(left.isSimple() && right.isSimple()){
        return integer;
    }
    report(E4, op);
    return err;
}


Type checkEqualityExpression(const Type &left, const Type &right, const string &op){
    if(left == err || right == err){
        return err;
    }
    
    Type l = left.promote();
    Type r = right.promote();

    if(l.specifier()==r.specifier() && l.indirection()==r.indirection()){
        return integer;
    }
    report(E4, op);
    return err;
}


Type checkRelationalExpression(const Type &left, const Type &right, const string &op){
    if(left == err || right == err){
        return err;
    }


    Type l = left.promote();
    Type r = right.promote();

    if(l.specifier()==r.specifier() && l.indirection()==r.indirection()){
        return integer;
    }
    report(E4, op);
    return err;
}


Type checkAdditiveExpression(const Type &left, const Type &right, const string &op){
    if(left == err || right == err){
        return err;
    }

    Type l = left.promote();
    Type r = right.promote();


    if(!checkComplete(l) || !checkComplete(r)){
        report(E10);
        return err;
    }
    if(l==integer && r==integer){
        return integer;
    }
    if(l.indirection()>0 && r==integer){
        return Type(left.specifier(), left.indirection());
    }
    if(op=="+"){
        if(l==integer && r.indirection()>0){
            return Type(l.specifier(), r.indirection());
        }
    }
    if(op=="-"){
        if(l.indirection()>0 && r.indirection()>0){
            return integer;
        }
    }
    report(E4,op);
    return err;
}


Type checkMultiplicativeExpression(const Type &left, const Type &right, const string &op){
    if(left == err || right == err){
        return err;
    }

    if(left.promote()==integer && right.promote()==integer){
        return integer;
    }
    report(E4, op);
    return err;
}


Type checkCastExpression(const Type &t, int pointers, const string &specifier){
    if(t == err){
        return err;
    }

    Type result = Type(specifier, pointers);
    // cout<<t<<" ------ "<<right<<endl;
    if(!t.isSimple() || !result.isSimple()){
        //cout<<t<<" ------ "<<result<<endl;
        report(E6);
        return err;
    }

    return result;
}


Type checkPrefixExpression(const Type &t, const string &op, bool &lvalue){
    if(t == err ){
        return err;
    }

    if(op=="*"){
        if(t.promote().indirection()==0){
            report(E5, op);
            return err;
        }
        if(!checkComplete(t)){
            report(E10);
            return err;
        }
        lvalue = true;
        return Type(t.specifier(), t.promote().indirection()-1);
    }
    if(op=="&"){
        //must be an lvalue!!!
        //cout<<"HERE!!!"<<endl;
        if(!lvalue){
            //cout<<"HI: "<<t<<endl;
            report(E3);
            return err;
        }
        lvalue = false;
        return Type(t.specifier(), t.indirection()+1);
    }
    if(op=="!"){
        if(!t.isSimple()){
            report(E5, op);
            return err;
        }
        lvalue = false;
        return integer;
    }
    if(op=="-"){
        if(t.promote()!=integer){
            report(E5, op);
            return err;
        }
        lvalue = false;
        return integer;
    }
    if(op=="sizeof"){
        if(t.isFunction() || !checkComplete(t)){
            report(E7);
            return err;
        }
        lvalue = false;
        return integer;
    }
    return err;
}

Type checkPostfixExpression(const Type &left, const Type &right, const string &op, bool &lvalue, const string &name){
    
    Symbol *sym;
    if(op=="["){
        //cout<<"LEEEEEEFFTTT: " <<left << " indirection: "<< left.indirection() <<endl;
        if(left==err || right == err){
            return err;
        }
        if(left.promote().indirection()==0){
            report(E4, "[]");
            return err;
        }
        if(!checkComplete(left)){
            report(E10);
            return err;
        }
        if(right.promote()!=integer){
            report(E4, "[]");
            return err;
        }
        lvalue = true;
        return Type(left.specifier(), left.promote().indirection()-1);
    }
    if(op=="."){
        if(left==err){
            return err;
        }
        
        if(!checkComplete(left)){
            report(E10);
            return err;
        }
        
        if(!fields.count(left.specifier()) || (sym=fields[left.specifier()]->find(name))==NULL){
            report(E4, op);
            return err;
        }

        if(sym->type().isArray()){
            lvalue=false;
        }
        return sym->type();
    }
    if(op=="->"){
        if(left==err){
            return err;
        }
        if(left.promote().indirection()==0){
            report(E4, op);
            return err;
        }
        if(!checkComplete(left)){
            report(E10);
            return err;
        }
        
        /*
        if(fields.count(left.specifier()) ){
            cout<<"SYmbol Found: "<<fields[left.specifier()]->lookup(name)->type()<<endl;
            cout<<fields[left.specifier()]; 
        }
        cout<<"INSIDE CHECKER!!!!  Trying to find: "<<name<<" Inside left.specifier: "<< left.specifier()<< endl;
        */
        if(!fields.count(left.specifier()) || (sym=fields[left.specifier()]->find(name))==NULL){
            report(E4, op);
            return err;
        }

        if(!sym->type().isArray()){
            lvalue=true;
        }else{
            lvalue = false;
        }
        return sym->type();
    }
    return err;    
}


Type checkPrimaryExpression(){
    //Deal with functions
    return err;
}

Type checkArguments(std::vector<Type> argList, Symbol *sym){
    if(! sym->type().isFunction()){
        report(E8);
        return err;
    }

    if(sym->type().isFunction() && !sym->type().parameters()){
        for(unsigned i = 0; i<argList.size(); ++i){
            if(! argList[i].isSimple()){
                report(E9);
                return err;
            }
        }
    }

    if(sym->type().isFunction() && sym->type().parameters()){
        //we check the number of parameters match
        std::vector<Type> params = *(sym->type().parameters());
        if(argList.size()!=params.size()){
            report(E9);
            return err;
        }

        for(unsigned i=0;i<argList.size();++i){
            if(!isCompatible(argList[i],params[i])){
                report(E9);
                return err;
            }
        }
    }

    //if everything passes, return type of function
    //return sym->type();
    return Type(sym->type().specifier(), sym->type().indirection());
}









