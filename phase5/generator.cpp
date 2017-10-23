#include <iostream>
#include "generator.h"

using namespace std;

void globalsGenerator(const Symbols &globalVariables)
{
	if(globalVariables.size()>0)
	{
		cout<<"\t.data"<<endl;
		for(int i =0; i<globalVariables.size();++i)
		{
			cout<<"\t"<<globalVariables[i]->name()<<", "<<globalVariables[i]->type().size()<<endl;
		}
	}
}

void Function::generate()
{
	Symbols vars = _body->declarations()->symbols();

	
	//Prologue
	cout<<_id->name()<<":"<<endl;
	cout<<"\tpushl \%ebp"<<endl;
	cout<<"\tmovl \%esp, \%ebp"<<endl;
	cout<<"\tsubl $"<<_id->name()<<".size, \%esp"<<endl;

	//Body
	unsigned i =0;
	int offset=0;
	for(i=0;i < _id->type().parameters()->size(); ++i)
	{
		vars[i]->setOffset(8+4*i);
	}
	for(;i<vars.size();++i)
	{
		offset -= vars[i]->type().size();
		vars[i]->setOffset(offset);
	}

	_body->generate();

	//Epilogue
	cout<<"\tmovl \%ebp, \%esp"<<endl;
	cout<<"\tpopl \%ebp"<<endl;
	cout<<"\tret"<<endl<<endl;

	

	cout<<"\t.globl "<<_id->name()<<endl;
	cout << "\t.set " <<_id->name()<< ".size, "<<-offset<<endl<<endl;
}

void Block::generate()
{
	for(int i=0; i<_stmts.size(); ++i)
	{
		_stmts[i]->generate();
	}
}

void Assignment::generate()
{
	cout<<"\tmovl ";
	_right->generate();
	cout<<", \%eax"<<endl;


	cout<<"\tmovl \%eax, ";
	_left->generate();
	cout<<endl; //need to print the name
}

void Call::generate()
{
	for(int i=_args.size()-1; i>=0; --i)
	{
		cout<<"\tpushl ";
		_args[i]->generate();
		cout<<endl;
	}

	cout<<"\tcall "<<_id->name()<<endl;
	cout<<"\taddl $"<<4*_args.size()<<", \%esp"<<endl;
}

void Number::generate()
{
    cout<<"$"<<_value;
}

void Identifier::generate()
{
	if(_symbol->offset()==0)
	{
		cout<<_symbol->name();
	}
	else
	{
		cout<<_symbol->offset()<<"(\%ebp)";
	}
}

