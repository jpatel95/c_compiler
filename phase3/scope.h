#ifndef SCOPE_H
#define SCOPE_H

#include "symbol.h"
#include <string>
#include <vector>


class Scope{
	typedef std::string string;
	typedef std::vector<Symbol *> Symbols;

	Scope *_enclosing;
	Symbols _symbols;

public:	
	Scope(Scope *enclosing=0);
	Scope *enclosing() const;
	const Symbols & symbols() const;
	void insert(Symbol *);
	void remove(const string &name);

	Symbol *findStructDef(const string &specifier)const;

	Symbol *find(const string &name)const;
	Symbol *lookup(const string &name)const;
};

#endif /*SCOPE_H*/