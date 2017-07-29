/* tepapa.cc -- TEPAPA main file
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

#include "eperl.h"
#include "strfriends.h"

PerlInterpreter*  EmbeddedPerl::my_perl = 0;
int EmbeddedPerl::use_count = 0;

EmbeddedPerl::EmbeddedPerl(bool f_embed) { 
	if (!use_count) {
		my_perl = perl_alloc();
		perl_construct(my_perl);
		}
	++ use_count ; 
	if (f_embed) {
		char *embedding[] = { "", "-e", "0" };
		perl_parse(my_perl, NULL, 3, embedding, NULL);
		perl_run(my_perl);
		}
	}

EmbeddedPerl::~EmbeddedPerl() { 
	-- use_count ; 
	if (!use_count) {
		perl_destruct(my_perl);
		perl_free(my_perl);
		}
	}
	
void EmbeddedPerl::run(const vector<string>& argv) {
	int argc = argv.size() ;
	
	char* perl_argv[ argc + 1 ] ;
	
	for(int i=0; i<argc; ++i)  perl_argv[i] = strdup( argv[i].c_str() );
	perl_argv[argc+1] = 0;
	
	perl_parse(my_perl, NULL, 2, perl_argv, (char**) NULL);
	perl_run(my_perl);
	for(int i=0; i<argc; ++i) free( perl_argv[i] );
	}

string EmbeddedPerl::eval(const string& statement) {
	STRLEN  len;
	SV *val = eval_pv(statement.c_str(), TRUE);
	string retval = SvPV(val, len);
	return retval;
	};

string quote_perl_string(const string& x) {
	if ( x.find('\'') == string::npos ) return string("'") + x + string("'") ;
	if ( x.find('"')  == string::npos ) return string("\"") + x + string("\"") ;
	if ( x.find('(')  == string::npos && x.find(')')  == string::npos ) return string("q(") + x + string(")") ;
	return string("<<'ENDDATA';\n") + x + string("ENDDATA\n\n");
	}

string quote_perl_regex_subst(const string& x, const string& y) {
	const string candidate_chars = "/|!@#$%^&*()_+~";
	string sdelim_char = "/";
	for(unsigned int i=0; i<candidate_chars.size(); ++i) {
		if ( x.find(candidate_chars[i]) != string::npos ) continue;
		if ( y.find(candidate_chars[i]) != string::npos ) continue;
		sdelim_char = candidate_chars[i];
		break;
		}
	return string("s") + sdelim_char + x + sdelim_char + y + sdelim_char ;
	}
	
string EmbeddedPerl::simple_replacement(const string& data, const vector< pair<string,string> >&  pattern_replacement_pairs) {
	string statement = string("my $data = ") + quote_perl_string(data) + ";"; 
	for (vector< pair<string,string> >::const_iterator it = pattern_replacement_pairs.begin(); it != pattern_replacement_pairs.end(); ++it) {
		statement += string("$data =~ ") + quote_perl_regex_subst(it->first , it->second) + string("g;\n");
		}
	statement += string("return $data;\n");
// 	printf("%s\n", statement .c_str());
	string retval = eval(statement);
	return retval ;
	}

string EmbeddedPerl::simple_replacement(const string& data, const string& pattern, const string& replacement) {
	string statement = string("my $data = ") + quote_perl_string(data) + ";"; 
	statement += string("$data =~ ") + quote_perl_regex_subst(pattern, replacement) + string("sg;\n");
	statement += string("return $data;\n");
// 	printf("%s\n", statement .c_str());
	string retval = eval(statement);
	return retval ;
	}
