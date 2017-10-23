/*
 * File:	generator.h
 *
 * Description:	This file contains the function declarations for the code
 *		generator for Simple C.  Most of the function declarations
 *		are actually member functions provided as part of Tree.h.
 */

# ifndef GENERATOR_H
# define GENERATOR_H
# include "Tree.h"
# include "lexer.h"
 
struct Label
{
	static unsigned counter;
	unsigned number;
	Label();
};

void generateGlobals(const Symbols &globals);
void assignTempOffset(Expression* expr);

# endif /* GENERATOR_H */
