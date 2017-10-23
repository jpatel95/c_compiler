#include "scope.h"
#include <string>
#include <iostream>
typedef std::vector<Symbol *> Symbols;


Scope::Scope(Scope *enclosing):_enclosing(enclosing){
	/* Constructor */
}

Scope *Scope::enclosing() const{
	return _enclosing;
}

const Symbols &Scope::symbols() const{
	return _symbols;
}

void Scope::insert(Symbol *sym){
	if(sym==NULL){
		std::cout<<"NULL pointer as parameter in scope's insert"<<std::endl;
		return;
	}

	if(find(sym->name())==0 || sym->name()==""){ // Name "" means its a struct
		_symbols.push_back(sym);
	}
	else{
		std::cout<<"Symbol already inside of symbol table for scope insert"<<std::endl;
	}
	return;
}

void Scope::remove(const string &name){
	for(unsigned int i=0;i<_symbols.size();++i){
		if(_symbols[i]->name()==name){
			_symbols.erase(_symbols.begin()+i);
		}
	}
	return;
}

Symbol *Scope::findStructDef(const string &specifier)const{
	for(unsigned int i=0;i<_symbols.size();++i){
		if(_symbols[i]->type().specifier()==specifier && _symbols[i]->type().isStruct() &&_symbols[i]->type().kind()==SCALAR && _symbols[i]->type().length()==-2){
			return _symbols[i];
		}
	}
	return 0;
}

Symbol *Scope::find(const string &name)const{
	for(unsigned int i=0;i<_symbols.size();++i){
		if(_symbols[i]->name()==name){
			return _symbols[i];
		}
	}
	return 0;
}

Symbol *Scope::lookup(const string &name)const{
	const Scope *currentScope=this;
	Symbol *findReturn;
	do{
		findReturn=currentScope->find(name);
		if(findReturn!=0){
			return findReturn;
		}
		currentScope=currentScope->_enclosing;
	}while(currentScope!=0);
	return 0;
}

