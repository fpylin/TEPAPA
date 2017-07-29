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

#include "tepapa-utils.h"
#include "ngram-pattern.h"

#include <string.h>

bool sort_by_pval(const TEPAPA_Result& a, const TEPAPA_Result& b ) {
	if  ( a.pval < b.pval ) return true;
	if  ( a.pval > b.pval ) return false;
	if  ( a.est  > b.est  ) return true;
	if  ( a.est  < b.est  ) return false;
	if  ( a.npos > b.npos ) return true;
	if  ( a.npos < b.npos ) return false;
	return ( strcmp( a.patt->to_string().c_str(), b.patt->to_string().c_str() ) < 0 );
	}

bool sort_by_est(const TEPAPA_Result& a, const TEPAPA_Result& b ) {
	return (a.est) > (b.est);
	}

bool sort_by_ngram_length_desc(const TEPAPA_Result& a, const TEPAPA_Result& b) {
	pattern* aptr = a.patt;
	pattern* bptr = b.patt;
	
	ngram_pattern*  anpptr = dynamic_cast<ngram_pattern*>(aptr);
	ngram_pattern*  bnpptr = dynamic_cast<ngram_pattern*>(bptr);
	
	if (   anpptr && ! bnpptr ) return true;
	if ( ! anpptr &&   bnpptr ) return false;
	if ( ! anpptr && ! bnpptr ) return false;
	return ( anpptr->size()  >  bnpptr->size() );
	}

binary_profile  match_type(const vector< TEPAPA_Result >& a,  hash_value hv_type_id) {
	binary_profile bp( a.size() );
	for(unsigned int i=0; i<a.size(); ++i) {
		bp[i] = ( (a[i].patt->get_type_id() == hv_type_id) ? true : false );
		}
	return bp;
	}

	
string find_executable(const string& name, const string& directory, const string& prefix, const string& suffix) {
	vector<string> names;
	names.push_back( name );
	names.push_back( strtolower(name) );
	names.push_back( strtoupper(name) );
	
	vector<string> directories;
	directories.push_back( string("./") );
	directories.push_back( directory + string("/") );

	vector<string> prefixes;
	prefixes.push_back( string("") );
	prefixes.push_back( prefix + string("-") );
	
	vector<string> suffixes;
	suffixes.push_back( string("") );
	suffixes.push_back( string(".") + suffix );
	
	
	for (uint a=0; a<directories.size(); ++a) {
		for (uint b=0; b<names.size(); ++b) {
			for (uint c=0; c<prefixes.size(); ++c) {
				for (uint d=0; d<suffixes.size(); ++d) {
					string cmdfn = strfmt("%s%s%s%s",
						directories[a].c_str(),
						prefixes[c].c_str(),
						names[b].c_str(),
						suffixes[d].c_str()
						);
					if ( file_exists(cmdfn) ) return cmdfn;
// 					printf("%d\t%d\t%d\t%d\t%s\n", a, b, c, d, cmdfn.c_str());
					}
				}
			}
		}
	
	
	return string();
	}


string find_executable(const vector<string>& prognames, const vector<string>& directories, const vector<string>& prefixes, const vector<string>& suffixes) {
// 	vector<string> prognames = { names, strtolower(name), strtoupper(name) };
	
	vector< vector<string> > dimensions   = { directories, prefixes, prognames, suffixes };
	vector< vector<string> > combinations = permute<string>(dimensions);
	
	for(unsigned int j=0; j<combinations.size(); ++j) {
		string path = join("", combinations[j]);
		if ( file_exists(path) ) return path;
		}
	return string();
	}
