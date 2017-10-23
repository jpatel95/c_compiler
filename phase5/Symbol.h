/*
 * File:	Symbol.h
 *
 * Description:	This file contains the class definition for symbols in
 *		Simple C.  At this point, a symbol merely consists of a
 *		name and a type, neither of which you can change.
 */

# ifndef SYMBOL_H
# define SYMBOL_H
# include "Type.h"

class Symbol {
    typedef std::string string;
    string _name;
    Type _type;
    int _offset;

public:
    Symbol(const string &name, const Type &type);
    const string &name() const;
    const Type &type() const;
    int offset() const;
    void setOffset(int num);
};

# endif /* SYMBOL_H */
