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
#ifndef __PARSE_H
#define __PARSE_H 1

#include <ctype.h>
#include <string.h>


inline bool __char_in_str(const char c, const char* chars)
	{
	for(register const char* z = chars; *z; ++z) if (*z == c) return true;
	return false;
	}

char* parse_c0_whitelines(char* str);
char* parse_c0_splitlines(char* str);
char* parse_c0_leadingspaces(char* str);

char* parse_g0_getline(char* str, char* buf);
	
char* parse_int(const char* str, int& var);
char* parse_double(const char* str, double& var);
char* parse_quoted_string(const char* str, char* var, char quote_char='"');

inline char* parse_skip_int(const char* str)
	{
	int dummy;
	register char* z = parse_int(str,dummy);
	return (char*) (z ? z : str);
	}

inline char* parse_skip_double(const char* str)
	{
	double dummy;
	register const char* z = parse_double(str,dummy);
	return (char*) (z ? z : str);
	}

	
inline char* parse_whitespaces(const char* z)
	{
	if(!isspace(*z)) return 0;
	while(isspace(*z)) z++;
	return (char*)z;
	}

inline char* parse_spaces(const char* z)
	{
	if(*z!=' ') return 0;
	while(*z==' ') z++; 
	return (char*)z;
	}

inline char* parse_spacetabs(const char* z)
	{
	if(*z!=' ' || *z!='\t') return 0;
	while(*z==' ' || *z=='\t') z++;
	return (char*)z;
	}

inline char* parse_char(const char* z, char ch)
	{
	return (char*)((*z == ch) ? z+1 : 0);
	}
	
inline char* parse_capture_char(const char* z, char& ch)
	{
	if(!*z) return 0;
	ch = *z;
	return (char*)(++z);
	}
	
inline char* parse_newline(const char* z)
	{
	return parse_char(z,'\n');
	}

inline char* parse_string(const char* str, const char* src)
	{
	do	{
		if (*str != *src) return 0;
		++str;
		++src;
		}
	while(*src);

	return (char*)str;
	}

inline char* parse_istring(const char* str, const char* src)
	{
	do	{
		if (toupper(*str) != toupper(*src)) return 0;
		++str;
		++src;
		}
	while(*src);

	return (char*)str;
	}

char* parse_strings_argv(const char* str, const char** src);
char* parse_istrings_argv(const char* str, const char** src);
char* parse_strings(const char *str, ...);
char* parse_istrings(const char *str, ...);


inline char* parse_skip_whitespaces(const char* z)
	{
	while(isspace(*z)) z++; 
	return (char*)z;
	}

inline char* parse_skip_word(const char* z)
	{
	if(!z || ! *z)  return 0;
	while(!isspace(*z) && *z) z++;
	z = parse_skip_whitespaces(z);
	return (char*)z;
	}
	
inline char* parse_capture_word(const char* z0, char* b)
	{
	register const char* z = z0;
	if(!z || ! *z)  return 0;
	while(!isspace(*z) && !ispunct(*z) && *z) {*b = *z; z++; b++;}
	if (z == z0) return 0;
	*b = 0;
	z = parse_skip_whitespaces(z);
	return (char*)z;
	}
	
inline char* parse_capture_punct(const char* z0, char* b)
	{
	register const char* z = z0;
	if(!z || ! *z)  return 0;
	while(ispunct(*z) && *z) {*b = *z; z++; b++;}
	if (z == z0) return 0;
	*b = 0;
	z = parse_skip_whitespaces(z);
	return (char*)z;
	}
	

inline char* parse_skip_spaces(const char* z)
	{
	while(*z==' ') z++; 
	return (char*)z;
	}

inline char* parse_skip_spacetabs(const char* str)
	{
	while(*str==' ' || *str=='\t') str++;
	return (char*)str;
	}	

inline char* parse_skip_char(const char* str, char ch)
	{
	if(*str == ch) str++;
	return (char*)str;
	}

inline char* parse_skip_newline(const char* str)
	{
	return parse_skip_char(str,'\n');
	}

inline char* parse_skip_string(const char* str, const char* src)
	{
	register const char* z0 = str;
	do	{
		if (*str != *src) return (char*)z0;
		++str;
		++src;
		}
	while(*src);
	return (char*)str;
	}

inline char* parse_capture_until_char(const char* str, const char chr, char* buf)
	{
	register const char* z = strchr (str, chr);
	register int n = z - str;
	if (z) {strncpy(buf, str, n); buf[n] = 0;}
	return (char*)z;
	}
	
char* parse_capture_until_chars(const char* str, const char* chrs, char* buf);
char* parse_capture_until_chars_z(const char* str, const char* chrs, char* buf);
char* parse_capture_until_string_z(const char* str, const char* string_delim, char* buf);
	
char* parse_skip_chars(const char* str, const char* chrs);
	
	

const char* parse_capture_matching_parenthesis(const char* str, char* buf, 
	const char chr_l='(', const char chr_r=')');
	


#endif
