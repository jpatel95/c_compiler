#ifndef TYPE_H
#define TYPE_H

#include <string>
#include <iostream>

enum Kind{
	SCALAR,ARRAY,FUNCTION
};

class Type{	
	typedef std::string string;
	
	string _specifier;
	unsigned _indirection;
	Kind _kind;
	int _length;
	bool _isStruct;

public:
	Type(const string &specifier, unsigned indirection, Kind kind, int length, bool isStr);
	unsigned indirection() const;
	Kind kind() const;

	//length = -2 for definition, -1 for declaration, 0-n for array
	int length() const;
	const string &specifier() const;
	bool isStruct() const;

	bool operator==(const Type &t); 
	bool operator!=(const Type &t); 
};

std::ostream &operator<<(std::ostream &ostr, const Type &type);
#endif /*TYPE_H*/