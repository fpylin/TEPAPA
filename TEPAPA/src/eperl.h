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
#ifndef __eperl_h
#define __eperl_h 1

#include <vector>
#include <string>

using namespace std;

#include <EXTERN.h>
#include <perl.h>


class EmbeddedPerl {
	static PerlInterpreter *my_perl;
	static int use_count;
	
	public:
	EmbeddedPerl(bool f_embed=false) ;
	
	virtual ~EmbeddedPerl() ;
	
	void run(const vector<string>& argv) ;
	
	string eval(const string& statement);
	
	string simple_replacement(const string& data, const vector< pair<string,string> >&  pattern_replacement_pairs);
	string simple_replacement(const string& data, const string& pattern, const string& replacement);
	};


#endif // __eperl_h
