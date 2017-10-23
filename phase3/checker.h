#ifndef CHECKER_H
#define CHECKER_H

#include <string>
#include "scope.h"

using namespace std;

void openScope();
void closeScope();
void declareVariable(const string &name, const string &specifier, unsigned indirection, Kind kind, int length, bool isStr);
void declareFunction(const string &name, const string &specifier, unsigned indirection, Kind kind, int length, bool isStr);
void defineFunction(const string &name, const string &specifier, unsigned indirection, Kind kind, int length, bool isStr);
void checkIfExists(const string &name);
void declareStruct(const string &name, const string &specifier, unsigned indirection, Kind kind, int length, bool isStr);
void defineStruct(const string &name, const string &specifier, unsigned indirection, Kind kind, int length, bool isStr);

#endif /*CHECKER_H*/