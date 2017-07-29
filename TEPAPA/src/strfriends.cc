/* 
 * Copyright (C) 2015-2016, Frank Lin - All Rights Reserved
 * Written by Frank P.Y. Lin 
 *
 * This file is part of TEPAPA, a Text mining-based Exploratory Pattern
 * Analyser for Prognostic and Associative factor discovery
 * 
 * TEPAPA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * TEPAPA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TEPAPA.  If not, see <http://www.gnu.org/licenses/>
 */
#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "strfriends.h"


string strfmt(const char* fmt, ...)
	{
	char buf[262144];
	va_list ap;
	va_start (ap, fmt);
	vsprintf (buf, fmt, ap);
	va_end (ap);
	return string(buf);
	}


int strfmtcat(string& s, const char* fmt, ...)
	{
	int n;
	char buf[262144];
	va_list ap;
	
	va_start (ap, fmt);
	n = vsprintf (buf, fmt, ap);
	va_end (ap);
	
	s += string(buf);
	
	return n;
	}


string join(const string& sep, const vector<string>& strs)
	{
	string s;
	for(unsigned int i=0; i<strs.size(); ++i)
		{
		s += strs[i];
		if ( i < strs.size() -1) s += sep;
		}
	return s;
	}


string strfmt(int v)
	{
	char buf[262144];
	sprintf (buf, "%d", v);
	return string(buf);
	}

string strfmt(double v)
	{
	char buf[262144];
	sprintf (buf, "%f", v);
	return string(buf);
	}

string pleural(const char* noun, int count)
	{
	if(count>1 || count<-1) return noun + string("s");
	return noun;
	}

string strtolower(const string& s) {
	int n = s.length(); 
	char buf[ n + 1 ];
	const char* ss = s.c_str();
	for(int i=0; i<n; ++i) buf[i] = tolower( ss[i] );
	buf[n] = 0;
	return string(buf);
	}

string strtoupper(const string& s) {
	int n = s.length(); 
	char buf[ n + 1 ];
	const char* ss = s.c_str();
	for(int i=0; i<n; ++i) buf[i] = toupper( ss[i] );
	buf[n] = 0;
	return string(buf);
	}

char* strdup_cc(const char* s)
	{
	if(!s) return 0;
	int n = strlen(s)+1;
	char* buf = new char[n];
	strcpy(buf,s);
	return buf;
	}

string escape_str(const string& s) {
	string out;
	out.reserve( s.size() * 2 );
	for(unsigned int i=0; i<s.length(); ++i) {
		switch (s[i]) {
			case '(': case ')':
			case '[': case ']':
			case '{': case '}':
			case '?': case '.':
			case '"':
			case '*':
			case '/':
			case '\'':
				out += '\\';
				break;
			default: 
				break;
			}
		out += s[i];
		}
	return out;
	}

