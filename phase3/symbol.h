#ifndef SYMBOL_H
#define SYMBOL_H

#include "type.h"
#include <string>


class Symbol{
	typedef std::string string;

	string _name;
	Type _type;


public:
	Symbol(const string &name, const string &specifier, unsigned indirection, Kind kind, int length, bool isStr);

	const string &name() const;
	Type type() const;
	int size() const;
	
	bool operator==(const Symbol &sym);
	bool operator!=(const Symbol &sym);

};

#endif /*SYMBOL_H*/