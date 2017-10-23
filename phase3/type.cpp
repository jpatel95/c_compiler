#include "type.h"
typedef std::string string;
typedef std::ostream ostream;


Type::Type(const string &specifier, unsigned indirection, Kind k, int length, bool isStr):_specifier(specifier)
	,_indirection(indirection), _kind(k), _length(length), _isStruct(isStr){
		/* Constructor */
}

unsigned Type::indirection() const{
	return _indirection;
}

Kind Type::kind() const{
	return _kind;
}

int Type::length() const{
	return _length;
}

const string & Type::specifier() const{
	return _specifier;
}

bool Type::isStruct() const{
	return _isStruct;
}


ostream& operator<<(ostream &ostr,const Type &type){
	int knd=type.kind();
	string kindString;
	if(knd==SCALAR){
		kindString="scalar";
	}
	else if(knd==ARRAY){
		kindString="array";
	}
	else if(knd==FUNCTION){
		kindString="function";
	}

	ostr<<"Specifier: "<<type.specifier()<<", indirection: "<<type.indirection()<<", Kind: "<<kindString
		<<", Length: "<<type.length()<<std::endl;
	return ostr;
}

bool Type::operator==(const Type &t){
	if(_kind!=t._kind)
		return false;
	if(_specifier!=t._specifier)
		return false;
	if(_indirection!=t._indirection)
		return false;
	if(_kind==ARRAY && _length!=t.length())
		return false;
	if(_isStruct!=t.isStruct())
		return false;
	
	return true;
}

bool Type::operator!=(const Type &t){
	return !operator==(t);
}
