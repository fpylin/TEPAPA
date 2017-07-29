#ifndef __VAR_H
#define __VAR_H 1

/*
 Variable class that holds a string type. This class facilitates
 data type conversion. 
*/

#include <string>
#include "hashtable.h"

using namespace std;

class Var: public string
	{
	public:
	Var(): string() {}
	Var(const Var& v): string(v) {}
	Var(const char* s): string(s) {}
	Var(const string& s): string(s) {}
// 	Var(bool b) {assign(b ? "t": "f");}
	Var(int i);
	Var(unsigned int i);
	Var(hash_value hv);
	Var(double d);
	~Var() {}
	
// 	operator bool() const ;
	operator unsigned int() const ;
	operator int() const ;
	operator double() const ;
	operator const hash_value() const ;
	operator const char* () const {return c_str();}
	operator char* () const {return (char*)c_str();}
	
	const char& operator [] (int i) const {return c_str()[i];}
	
// 	Var& operator = (bool b);
	Var& operator = (int i);
	Var& operator = (unsigned int i);
	Var& operator = (hash_value& hv);
	Var& operator = (double d);
	
	bool is_null();
	};

typedef Var var; // really dirty hack

#endif
