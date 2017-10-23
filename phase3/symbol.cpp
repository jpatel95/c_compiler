#include "symbol.h"

Symbol::Symbol(const string &name, const string &specifier, unsigned indirection, Kind k, int length, bool isStr):_name(name),
	_type(specifier, indirection, k, length, isStr){ 
	/* Constructor */
	 //_type(specifier, indirection, kind);
}

const std::string & Symbol::name() const{
	return _name;
}


Type Symbol::type() const{
	return _type;
}

bool Symbol::operator==(const Symbol &sym){ 
	if(_name==sym.name() && _type==sym.type()){
		return true;
	}
	return false;
}

bool Symbol::operator!=(const Symbol &sym){ 
	return !operator==(sym);
}