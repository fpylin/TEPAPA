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
#include <stdio.h>

#include <math.h>
#include <stdlib.h>
#include <stdarg.h>

#include "parse.h"

char* parse_c0_whitelines(char* str)
	{
	// Regular Expression: [ ]*\n
	int nns = 0;	// number of non-whitespace
	int i=0, j=0, i0;

	do	{
		i0 = i;
		for(nns=0; str[i] && str[i]!='\n'; i++) 
			if (isspace(str[i])) nns++;
		
		if(nns)
			for(j=i0; str[j] && str[j]!='\n'; j++, i0++)
				str[j] = str[i0];
				
		str[j++] = str[i];
		}
	while(str[i++]);

	return str;
	}
	
char* parse_c0_splitlines(char* str)
	{

	// Regular Expression: ... .*\\\n.* ... => .*.*
	int i, j;

	for(i=0, j=0; str[i]; i++, j++)
		{
		if (str[i]=='\\' && str[i+1]=='\n')
			{
			i++;
			j--;
			continue;
			}
		str[j] = str[i];
		}

	str[j]=0;
	return str;
	}

char* parse_c0_leadingspaces(char* str)
	{
	// Regular Expression: ... .*\\\n.* ... => .*.*
	int i=0, j;
	
	while(isspace(str[i])) i++;
	
	for(j=0; str[i]; i++, j++)
		{
		str[j] = str[i];
		if (str[i]=='\n')
			{
			while(isspace(str[i+1])) i++;
			continue;
			}
		}

	str[j]=0;
	return str;
	}



char* parse_g0_getline(char* str, char* buf)
	{
//	int i, j;
	int n;
	char* s;
	if (!*str) return 0;
	s = strchr(str,'\n');
	if (!s) 
		{
		strcpy(buf, str);
		return 0;
		}
	else
		{
		n = s-str;
		strncpy (buf, str, n);
		buf[n]=0;
		}

	return str + n + 1;
	}



char* parse_int(const char* str, int& var)
	{
	// Regular Expression: [\+-]?[0-9]+
	int bi=0;
	int i=0;
	char buffer[256];
	
	switch (str[i])
		{
		case '+':
		case '-': 	buffer[i] = str[i]; i++; break;
		default: break;
		}
		
	for(bi=i; isdigit(str[i]); i++) buffer[i]=str[i];
	if(i==bi) return 0;
	
	buffer[i]=0;
	var = atoi (buffer);
	
	return (char*)(str+i);
	}

char* parse_double(const char* str, double& var)
	{
	// Regular Expression: 	[-+]?([0-9]*\.[0-9]+)([eE][-+]?[0-9]+)?
	register int bi=0;
	register int i=0;
	char buffer[1024];
	
	switch (str[i])
		{
		case '+':
		case '-': 	buffer[i] = str[i]; i++; break;
		default: break;
		}
	for(bi=i; isdigit(str[i]); i++) buffer[i]=str[i];
	if (str[i] == '.')
		{
		buffer[i++]='.';
		for (; isdigit(str[i]); i++) buffer[i]=str[i];
		}
	if(i==bi) return 0;
	if (str[i] == 'e' || str[i] == 'E')
		{
		buffer[i]=str[i];
		++i;
		switch (str[i])
			{
			case '+':
			case '-': 	buffer[i] = str[i]; i++; break;
			default: break;
			}
		for(bi=i; isdigit(str[i]); i++) buffer[i]=str[i];
		if(i==bi) return 0;
		}		
	
	buffer[i]=0;
	var = atof (buffer);
	
	return (char*)(str+i);
	}


char* parse_quoted_string(const char* str, char* var, char quote_char)
	{
	// Regular Expression: \"[:print:]\"
// 	register int i, j;
	register const char* z=str;
	register char* y=var;

	if(*z != quote_char) return 0; else ++z;
	
	while(*z)
		{
		*y = *z;
		if (*z == quote_char)
			{
			if ( *(z-1) != '\\') break;
			else *(y-1)= quote_char;
			}
		++z;
		++y;
		}
	*y = 0;

	return (char*)(z+1);
	}

char* parse_strings_argv(const char* str, const char** src)
	{
	for (int i=0; src[i]; ++i)
		{
		register const char* z= parse_string(str, src[i]);
		if (z) return (char*)z;
		}
	return 0;
	}

char* parse_istrings_argv(const char* str, const char** src)
	{
	for (int i=0; src[i]; ++i)
		{
		register const char* z= parse_istring(str, src[i]);
		if (z) return (char*)z;
		}
	return 0;
	}


char* parse_strings(const char *str, ...)
	{
	va_list ap;
	va_start(ap, str);
	register const char* fmt;
	while ( (fmt = va_arg(ap, const char *)) )
		{
		register const char* z= parse_string(str, fmt);
		if (z) 
			{
			va_end(ap);
			return (char*)z;
			}
		}
	va_end(ap);
	return 0;
	}

char* parse_istrings(const char *str, ...)
	{
	va_list ap;
	va_start(ap, str);
	register const char* fmt;
	while ( (fmt = va_arg(ap, const char *)) )
		{
		register const char* z= parse_string(str, fmt);
		if (z) 
			{
			va_end(ap);
			return (char*)z;
			}
		}
	va_end(ap);
	return 0;
	}



char* parse_capture_until_chars(const char* str, const char* chrs, char* buf)
	{
	register char c;
	register const char *z = str;
	for (z = str; (c = *z) && ! __char_in_str(c, chrs); ++buf, ++z) *buf = c;
	*buf = 0;
	if (z == str) return 0;
	return (char*)z;
	}

char* parse_capture_until_chars_z(const char* str, const char* chrs, char* buf)
	{
	register char c;
	register const char *z = str;
	for (z = str; (c = *z) && ! __char_in_str(c, chrs); ++buf, ++z) *buf = c;
	*buf = 0;
// 	if (z == str) return 0;
	return (char*)z;
	}


char* parse_capture_until_string_z(const char* str, const char* string_delim, char* buf)
	{
	register const char *z = str;
	register const char *y = strstr(str, string_delim);
	
// 	printf("> %s\n", str);
// 	printf("> %s\n", y);
	if (!y) {
		strcpy(buf, str);
		z += strlen(str);
		}
	else {
		int len = y-z;
		strncpy(buf, str, len);
		buf[len] = 0;
		z += len + strlen(string_delim) ;
		}
// 	printf("> %s\n", z);
	return (char*)z;
	}


const char* parse_capture_matching_parenthesis(const char* str, char* buf, const char chr_l, const char chr_r)
	{
	if (!str || *str != chr_l) return 0;
	
	register unsigned int level = 1;
	register const char *z;
	register char *b;
	for(z = str+1, b = buf; *z ; ++z, ++b)
		{
		if (*z==chr_l) ++level; else if (*z==chr_r) --level;
		if (!level) {++z; break;}
		*b = *z;
		}
	if (level) return 0;
	*b = 0;
	return z;
	}
	

char* parse_skip_chars(const char* str, const char* chrs)
	{
	while(*str && strchr(chrs, *str)) ++str;
	return (char*)str;
	}


// #include <stdio.h>
// 
// int main(void)
// 	{
// 	char buf[65536];
// 	char buf1[65536];
// 	
// 	while( gets(buf) )
// 		{
// 		const char* z = buf;
// 		
// 		z = parse_capture_matching_parenthesis(z, buf1, '[', ']');
// 		
// 		if (z)
// 			{
// 			puts(buf1);
// 			puts(z);
// 			}
// 		else
// 			{
// 			puts("---");
// 			}
// 		}
// 	
// 	}
