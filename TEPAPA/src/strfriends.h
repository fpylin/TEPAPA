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
#ifndef __STRFRIENDS_H
#define __STRFRIENDS_H 1

#include <string>
#include <vector>
#include <string.h>
#include <ctype.h>

using namespace std;

string strfmt(const char* fmt, ...);
string strfmt(int v);
string strfmt(double v);

int strfmtcat(string& s, const char* fmt, ...);

string join(const string& sep, const vector<string>& strs);

string pleural(const char* noun, int count);

string strtolower(const string& s);
string strtoupper(const string& s);

char* strdup_cc(const char* s);

string escape_str(const string& s) ;


#ifndef _GNU_SOURCE
#include <stdlib.h>
#include <string.h>

#define strdupa(s)\
    (__extension__\
        ({                                            \
            __const char *__old = (s);                \
            size_t __len = strlen (__old) + 1;        \
            char *__new = (char *) alloca (__len);    \
            (char *) memcpy (__new, __old, __len);    \
        }))
#endif // not _GNU_SOURCE



inline char* strrtrim(char* buf) {
	char* z = buf + strlen(buf) - 1;
	while (z >= buf && isspace(*z)) {
		*z = 0;
		-- z;
		}
	return buf;
	}


inline void remove_comments(char* buf) {
	register char* x = buf;
	while( (x = strchr(x, '#') ) ) {
		while( *x != '\n' && *x ) {
			*x = ' ';
			++x;
			}
		++x;
		}
	}

	

#endif
