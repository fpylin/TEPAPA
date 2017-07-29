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
#include <algorithm>

#include "ngram-pattern.h"
#include "tepapa.h"

hash_value   ngram_pattern::ngram_pattern_hv_type_id = 0;




bool ngram_pattern::match(const token_string&  ts) const {
	
	const vector<int>& first_token_locs = ts.find_pos( front() );
	
	token_string::const_iterator s = ts.begin();
	
	for (unsigned int z=0; z<first_token_locs.size(); ++z) {
		
		token_string::const_iterator i = s + first_token_locs[z];
		
		bool f_terminate = false;
		bool f_matched = true;
		
		for(unsigned int j=0; j<size(); ++j) {
			
			token_string::const_iterator ij = i+j;
			
			if ( ij == ts.end() ) { f_matched=false; f_terminate=true; break; }
			
			hash_value hv_ng_j = *( begin() + j);
			
			// if wants wildcard
// 			fprintf( stderr, "j=%d\t%d\t%s\n", j, (i+j)->primary(), ght[ (i+j)->primary() ] );
			if ( f_use_wildcard && ( hv_ng_j == hv_wildcard) ) continue;
			
// 			if ( hv_separators.find(ij->primary()) != hv_separators.end() ) { f_matched=false; f_terminate=true; break; }
			
			if (f_deep_cmp) {// deep comparison, traversing through all subclasses
				if ( ! ( ij->has_class( hv_ng_j ) ) ) { f_matched=false; break; }
				}
			else { // compare the primary token only.
				if ( ij->primary() != hv_ng_j ) { f_matched=false; break; }
				}
			}
		if (f_matched) return true;
		if (f_terminate) break;
		}
	return false;
	}
	
	
token_string::const_iterator ngram_pattern::find(const token_string&  ts, token_string::const_iterator  start) const {
	const vector<int>& first_token_locs = ts.find_pos( front() );
	
	token_string::const_iterator s = ts.begin();
	
	for (unsigned int z=0; z<first_token_locs.size(); ++z) {
		token_string::const_iterator i = s + first_token_locs[z];
		
		if (i < start) continue;
		bool f_terminate = false;
		bool f_matched = true;
		for(unsigned int j=0; j< size(); ++j) {
			hash_value hv_ng_j = *( begin() + j);
			
			if ( i+j == ts.end() ) { f_matched=false; f_terminate=true; break; }
			if ( ! ( (i+j)->has_class( hv_ng_j ) ) ) { f_matched=false; break; }
			}
		if (f_matched) return i;
		if (f_terminate) break;
		}
	return ts.end();
	}


vector<int> ngram_pattern::find_all(const token_string&  ts) const {
	const vector<int>& first_token_locs = ts.find_pos( front() );
	vector<int> retval ;
	
	token_string::const_iterator s = ts.begin();
	
	for (unsigned int z=0; z<first_token_locs.size(); ++z) {
		token_string::const_iterator i = s + first_token_locs[z];
		bool f_matched = true;
		for(unsigned int j=0; j<size(); ++j) {
			hash_value hv_ng_j = *( begin() + j);
			
			if ( i+j == ts.end() ) { f_matched=false; break; }
			if (f_deep_cmp) {
				if ( ! ( (i+j)->has_class( hv_ng_j ) ) ) { f_matched=false; break; }
				}
			else 
				{
				if ( (! (i+j)->size()) ) { f_matched=false; break; }
				if ( (i+j)->primary() != hv_ng_j )  { f_matched=false; break; }
				}
			}
		if (f_matched) retval.push_back( first_token_locs[z] );
		}
	return retval ;
	}








int ngram_pattern::sim(const ngram_pattern& ssp) const {
	vector<hash_value> ssp0;
	vector<hash_value> ssp1;
	ssp0 = *this;
	ssp1 = ssp;
	
	sort( ssp0.begin(), ssp0.end() );
	sort( ssp1.begin(), ssp1.end() );
	
	unsigned int i=0, j=0;
	int neq = 0;
	while( (i < ssp0.size()) && (j < ssp1.size() ) ) {
		if ( ssp0[i] < ssp1[j]) {
			++i;
			}
		else if ( ssp0[i] > ssp1[j]) {
			++j;
			}
		else { // (ssp0[i] == ssp0[j]) 
			++neq;
			++i;
			++j;
			}
		}
	return neq;
	}


string ngram_pattern::to_string(const string& delim) const {
	string substr_str;
	
	for(unsigned int i=0; i<size(); ++i) { 
		if ( substr_str.length() > 0 ) substr_str += delim;
		hash_value h = *(begin()+i)  ;
		substr_str += ght[ h ] ;
		}
	return substr_str ;
	}
	


bool ngram_pattern::has_subpat(const matchable_pattern& ssmp) const {
	const matchable_pattern* mp = &ssmp;
	const ngram_pattern* sspp = dynamic_cast<const ngram_pattern*>(mp);
	if (!sspp) return false;
	
	const ngram_pattern& ssp = *sspp ;
	
	if (!ssp.size()) return true;
	for(const_iterator i=begin(); i!=end(); ++i) {
		if ( *i == ssp.front() ) {
			for (unsigned int j=0; j<=ssp.size(); ++j) {
				if ( j == ssp.size() ) return true;
				if ( (i+j) == end() ) break;
				if ( *(i+j) != ssp[j] ) break;
				}
			}
		}
	return false;
	}


double ngram_pattern::atfidf(const sample_list& sl) const {
	double x = 0;
	for(const_iterator i=begin(); i!=end(); ++i) {
		map<hash_value, double>::const_iterator j = sl.atfidf.find( *i );
		if ( j != sl.atfidf.end() ) x += j->second ;
		}
	return x;
	}

string ngram_pattern::gen_pcre() const { 
	string s;
	for(const_iterator i=begin(); i!=end(); ++i) {
		if (s.length()) s += "\\s*";
		s += escape_str( ght[ *i ] );
		}
	return s;
	}

	
///////////////////////////////////////////////////////////////////////////

hash_value extractable_ngram_pattern::hv_extractable_ngram_pattern_id;


string extractable_ngram_pattern::to_string() const {
	string substr_str;
	
	for(unsigned int i=0; i<size(); ++i) { 
		if ( substr_str.length() > 0 ) substr_str += "  ";
		hash_value h = *(begin()+i)  ;
		if ((int)i == tag_i) substr_str += "?";
		substr_str += ght[ h ] ;
		}
	return substr_str ;
	}


#include "math.h"

double extractable_ngram_pattern::eval(const token_string&  ts) const {
	double retval = nan("");
		
	token_string::const_iterator z = find( ts );
		
	if ( z == ts.end() ) return retval;
	
	// has to be an unique pattern .... 
	token_string::const_iterator z1 = z;  ++z1;
    
	// ... otherwise we will bail out
	if ( find(ts, z1) != ts.end() ) return retval;
	
	retval = atof( ght[ (z + tag_i) -> primary() ] );
	
	return retval;
	}

	
ngram_pattern::ngram_pattern(const string& serialised_str): matchable_pattern(), vector<hash_value>() {
	initialise();
}

// #define NGRAM_PATTERN_TAG_O  "<elem>"
// #define NGRAM_PATTERN_TAG_E  "</elem>"

// string ngram_pattern::serialise_proper() const {
// 	string retval;
// 	for(const_iterator i=begin(); i!=end(); ++i) {
// 		retval += string(NGRAM_PATTERN_TAG_O);
// 		retval += ght[*i] ;
// 		retval += string(NGRAM_PATTERN_TAG_E);
// 		}
// 	return retval;
// 	}

// #include "tepapa-msg.h"

// size_t ngram_pattern::unserialise_proper(const string& serialised_str, size_t pos) {
// 	clear();
// 	size_t b = pos ;
// 	size_t n = serialised_str.length(); 
// 	size_t tag_o_len = string(NGRAM_PATTERN_TAG_O).length();
// 	size_t tag_e_len = string(NGRAM_PATTERN_TAG_E).length();
// 	
// 	while ( serialised_str.find( NGRAM_PATTERN_TAG_O, b ) == b ) {
// 		b += tag_o_len ;
// 		
// 		size_t e = serialised_str.find( NGRAM_PATTERN_TAG_E, b ) ;
// 	
// 		if ( e == std::string::npos ) { 
// 			msgf(VL_FATAL, "Unable to unseralise object");
// 			return e;
// 			}
// 		else {
// 			string elem = serialised_str.substr(b, e-b);
// 			hash_value hv_elem = ght( elem ); 
// 			push_back(hv_elem);
// 			b += tag_e_len ;
// 			}
// 		}
// 	return b;
// 	}
