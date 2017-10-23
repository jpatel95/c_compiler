/*
 * File:	lexer.h
 *
 * Description:	This file contains the public function and variable
 *		declarations for the lexical analyzer for Simple C.
 */

# ifndef LEXER_H
# define LEXER_H

extern int lineno, numerrors;

enum Token{
	EQL='=',LTN='<', GTN='>',ADD='+',SUB='-',MUL='*',DIV='/',REM='%',CAST,ADDR='&',NOT='!',DOT='.',LEFTPAREN='(',
	RIGHTPAREN=')',LEFTBRACKET='[',RIGHTBRACKET=']',LEFTBRACE='{',RIGHTBRACE='}',COMMA=',',SEMICOLON=';',
	COLON=':', ERROR=0, ID=256,STRING,NUM,CHARACTER,ARROW,INCR,DECR, EQLEQL, OR,AND,LEQ,GEQ, SIZEOF,INDEX,
	NEQ,AUTO,BREAK,CASE,CHAR,CONST,CONTINUE,DEFAULT,DO,DOUBLE,ELSE,ENUM,EXTERN,FLOAT,FOR,GOTO,IF,INT,LONG,
	REGISTER,RETURN,SHORT,SIGNED,STRUCT,SWITCH,TYPEDEF,UNION,UNSIGNED,VOID,VOLATILE,WHILE,JUNK
};

Token lexan(std::string &lexbuf);
void report(const std::string &str, const std::string &arg = "");

# endif /* LEXER_H */
