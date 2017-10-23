#include "checker.h"
#include "lexer.h"


Scope *head=NULL; //File Scope
Scope *current=NULL; //Current Scope

void openScope(){
	current = new Scope(current);
	if(head==NULL){
		head=current;
	}
}

void closeScope(){
	if(current==NULL || current==0){
		return;
	}
	Scope *newCurrent=current->enclosing();
	delete current;
	current = newCurrent;
}

void declareVariable(const string &name, const string &specifier, unsigned indirection, Kind kind, int length, bool isStr){
	Symbol *symFound = current->find(name);
	Symbol *sym = new Symbol(name, specifier, indirection, kind, length, isStr);
	cout<<name<<" "<<sym->type();
	if(symFound==NULL){
		current->insert(sym);
	}
	else if(current!=head){
		cout<<"Redeclaration"<<endl;
		report("redeclaration of '%s'", name);
		delete sym;
	}
	else if(symFound->type() != sym->type()){
		cout<<"Conflicting Type"<<endl;
		report("conflicting types for '%s'", name);
		delete sym;
	}
}

void declareFunction(const string &name, const string &specifier, unsigned indirection, Kind kind, int length, bool isStr){
	Symbol *symFound = head->find(name);
	Symbol *sym = new Symbol(name, specifier, indirection, kind, length, isStr);
	if(!symFound && indirection==0 && isStr==true){ //You need to return a pointer error
		cout<<"pointer type required for function definiton"<<endl;
		report("pointer type required for '%s'", name);
		delete sym;
		return;
	}
	if(symFound==NULL){
		head->insert(sym);
	}
	else if(symFound->type() != sym->type()){
		cout<<"Conflicting Type for function declaration"<<endl;
		report("conflicting types for '%s'", name);
		delete sym;
	}
	return;
}

void defineFunction(const string &name, const string &specifier, unsigned indirection, Kind kind, int length, bool isStr){
	Symbol *symFound = head->find(name);
	Symbol *sym = new Symbol(name, specifier, indirection, kind, length, isStr);
	if(symFound!=NULL){ //Something has the same name
		if(symFound->type().length()==-2){//If the sym found was a definiton because
			cout<<"redefinition of function"<<endl;
			report("redefinition of '%s'", name);
			head->remove(name);
			head->insert(sym);
			return;
		}
		else if(symFound->type()!=sym->type()){ //If the found was a declaration
			cout<<"Conflicting Type for function definition"<<endl;
			report("conflicting types for '%s'", name);
			head->remove(name);
			head->insert(sym);
			return;
		}
	}
	if(indirection==0 && isStr==true){ //You need to return a pointer error
		cout<<sym->type();
		cout<<"pointer type required for function definiton"<<endl;
		report("pointer type required for '%s'", name);
		head->insert(sym);
		return;
	}
	//It was not found, so add it
	head->insert(sym);
	return;
}

void checkIfExists(const string &name){
	Symbol *symFound;
	if((symFound = current->lookup(name))==NULL){
		cout<<"Variable undeclared"<<endl;
		report("'%s' undeclared", name);
	}
}

//Length=0 mean paramter, =-1 means declaration, -2 means definition
void declareStruct(const string &name, const string &specifier, unsigned indirection, Kind kind, int length, bool isStr){
	Symbol *symFound = current->find(name);
	Symbol *structFound = head->findStructDef(specifier);
	Symbol *sym = new Symbol(name, specifier, indirection, kind, length, isStr);

	if(indirection==0 && length==0 && isStr==true){ //If it was a parameter, it needs to be a pointer
		cout<<"pointer type required for "<<endl;
		report("pointer type required for '%s'", name);
		delete sym;
		return;
	}

	if(symFound!=NULL){ //Found something with same ID
		cout<<"Inside symfound!=NULL "<<symFound->type();
		if(current!=head && symFound->name()==sym->name()){
			report("redeclaration of '%s'", name);
			delete sym;
			return;
		}
		else if(symFound->type()!=sym->type()){ //Check for conflicting type
			cout<<"Conflicting Type for struct declaration"<<endl;
			report("conflicting types for '%s'", name);
			delete sym;
			return;
		}
	}
	//Else the name was found
	if(structFound==NULL){ //Struct hasn't been defined yet
		cout<<"HERE NOT FOUND"<<endl;
		if(indirection==0){ 
			cout<<"Incomplete Type"<<endl;
			report("incomplete type for '%s'", name);
			current->insert(sym);
			return;
		}
	}
	else{
		cout<<"Struct FOUND = TRUE"<<endl;
	}
	current->insert(sym);
	return;
}



/*
Do newer redefinitions overpower old definitions???!?!???!?
*/
void defineStruct(const string &name, const string &specifier, unsigned indirection, Kind kind, int length, bool isStr){
	Symbol *structFound = head->findStructDef(specifier);
	Symbol *sym = new Symbol(name, specifier, indirection, kind, length, isStr);

	if(structFound==NULL){ //Hasn't been defined yet so you need to add
		head->insert(sym);
	}
	else{ //struct has been found
		cout<<"redefinition of struct"<<endl;
		report("redefinition of '%s'", specifier);
	}

	return;
}







