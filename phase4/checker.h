/*
 * File:	checker.h
 *
 * Description:	This file contains the public function declarations for the
 *		semantic checker for Simple C.
 */

# ifndef CHECKER_H
# define CHECKER_H
# include "Scope.h"

Scope *openScope();
Scope *closeScope();

void defineStructure(const std::string&name, Scope *scope);

Symbol *defineFunction(const std::string &name, const Type &type);
Symbol *declareFunction(const std::string &name, const Type &type);
Symbol *declareParameter(const std::string &name, const Type &type);
Symbol *declareVariable(const std::string &name, const Type &type);
Symbol *checkIdentifier(const std::string &name);



bool isCompatible(const Type &left, const Type &right);
bool checkComplete(const Type &t);
void checkWhile(const Type &t);
void checkIf(const Type &t);
void checkAssignment(const Type &left, const Type &right, bool &lvalue);
void checkReturn(const Type &left, const Type &right);

Type checkLogicalExpression(const Type &left, const Type &right, const std::string &op);
Type checkEqualityExpression(const Type &left, const Type &right, const std::string &op);
Type checkRelationalExpression(const Type &left, const Type &right, const std::string &op);
Type checkAdditiveExpression(const Type &left, const Type &right, const std::string &op);
Type checkMultiplicativeExpression(const Type &left, const Type &right, const std::string &op);
Type checkCastExpression(const Type &t, int pointers, const std::string &specifier);
Type checkPrefixExpression(const Type &t, const std::string &op, bool &lvalue);
Type checkPostfixExpression(const Type &left, const Type &right, const std::string &op, bool &lvalue, const std::string &name="");
Type checkPrimaryExpression();
Type checkArguments(std::vector<Type> argList, Symbol *sym);

# endif /* CHECKER_H */
