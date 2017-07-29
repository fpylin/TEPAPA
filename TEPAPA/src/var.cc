#include "var.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>


Var::Var(double d)
	{
	char buf[256];
	sprintf(buf, "%f", d);
	assign(buf);
	}

Var::Var(int i)
	{
	char buf[256];
	sprintf(buf, "%d", i);
	assign(buf);
	}

Var::Var(unsigned int i)
	{
	char buf[256];
	sprintf(buf, "%u", i);
	assign(buf);
	}

Var::Var(hash_value hv) {
	char buf[256];
	sprintf(buf, "%lu", hv);
	assign(buf);
	}

// Var::operator bool() const
// 	{
// 	if(!strcasecmp(c_str() ,"TRUE")) return true;
// 	if(!strcasecmp(c_str() ,"T")) return true;
// 	return false;
// 	}
	
Var::operator int() const
	{
	return atoi(c_str());
	}
	
Var::operator unsigned int() const
	{
	return atoi(c_str());
	}
	
Var::operator double() const
	{
	return atof(c_str());
	}

Var::operator const hash_value() const
	{
	return atoi(c_str());
	}


// Var& Var::operator = (bool b)
// 	{
// 	assign (b ? "t": "f");
// 	return *this;
// 	}
	
Var& Var::operator = (double d)
	{
	char buf[256];
	sprintf(buf, "%f", d);
	assign(buf);
	return *this;
	}

Var& Var::operator = (int i)
	{
	char buf[256];
	sprintf(buf, "%d", i);
	assign(buf);
	return *this;
	}

Var& Var::operator = (unsigned int i)
	{
	char buf[256];
	sprintf(buf, "%u", i);
	assign(buf);
	return *this;
	}

Var& Var::operator = (hash_value& hv)
	{
	char buf[256];
	sprintf(buf, "%lu", hv);
	assign(buf);
	return *this;
	}

bool Var::is_null()
	{
	if (!strcasecmp(c_str(), "NULL")) return true;
	if (!length()) return true;
	return false;
	}

/*
int main (void)
	{
	Var d1;
	Var d2;
	d1 = "123456789.123435";
	printf ("%08X", double(d1));
	}
*/
