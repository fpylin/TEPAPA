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
#include "regex.h"
#include "tepapa.h"
#include <algorithm>

hash_value regex::regex_pattern_hv_type_id = 0;

std::string sequence::to_str() const {
	string s;
	for(const_iterator it=begin(); it!=end(); ++it) {
		if ( s.size() ) s += "  "; 
		s += ght[*it];
		}
	return s;
	}

map<hash_value, int> sequences::count_tokens() const {
	map<hash_value, int>  retval;
	for(const_iterator it=begin(); it!=end(); ++it) {
		set<hash_value>  hv_set;
		for(sequence::const_iterator j=it->begin(); j!=it->end(); ++j) hv_set.insert( *j );
		for(set<hash_value>::const_iterator k=hv_set.begin(); k!=hv_set.end(); ++k) retval[*k] ++;
		}
	return retval;
	}

std::string sequences::to_str(const string& prefix) const {
	string s;
	for( const_iterator it = begin(); it != end() ; ++it ) {
		s += prefix + it->to_str() + string("\n");
		}
	return s;
	}

void sequences::push_back_unique(const sequence& s) {
	for (const_iterator it=begin(); it!=end(); ++it) {
		if ( s == (*it) ) return;
		}
	push_back(s);
	}

int sequences::split_by(hash_value hv_sep, sequences& out_group1, sequences& out_group2) const{
	out_group1.clear();
	out_group2.clear();
	
	int n=0;
	for(const_iterator it=begin(); it!=end(); ++it, ++n) {
		bool f_found = false;
		for(auto j=it->begin(); j<it->end(); ++j) {
			hash_value h = *j;
			if (h != hv_sep) continue;
			f_found = true;
			break;
			}
		if(f_found) {
			out_group1.push_back(*it);
			}	
		else {
			out_group2.push_back(*it);
			}
		}
	
	return n;
	}

void sequences::uniq() {
	set<sequence>  exists;
	
	sequences  retval;
	for (const_iterator it=begin(); it!=end(); ++it) {
		if ( exists.find(*it) == exists.end() ) {
			retval.push_back(*it);
			exists.insert(*it);
			}
		}
		
	*this = retval;
	}

int regex_element::deepsilon(void) {  // remove all empty options
	vector< sequence >::iterator it = options.begin(); 

	int nremoved = 0;
	while( it != options.end() ) {
		if ( it->size() == 0 )  {
			it = options.erase(it);
			++nremoved ;
			}
		else {
			++it;
			}
		}
	return nremoved;
	}


void regex_element::sort() {
	::sort(options.begin(), options.end());
	}


// string regex::to_str(const char* delim) const {
// 	string s;
// 	for(const_iterator i=begin(); i!=end(); ++i) {
// 		bool do_paren = 0;
// 		bool do_qmark = 0;
// 		int on = i->options.size();
// 		if ( i != begin() ) s += "  ";
// 		if ( on >= 2 ) {
// 			do_paren = 1;
// 			if ( ( i->options[ on - 1 ].size() == 0 ) ) {
// 				if ( on == 2 ) do_paren = 0;
// 				do_qmark = 1;
// 				}
// 			}
// 		if (do_paren) s += "\e[1;33m(\e[0m";
// 		for(int j=0; j<on ; ++j) {
// 			int nk = i->options[j].size() ;
// 			if ( nk == 0 ) continue;
// 			if (j != 0) s += "\e[1;33m|\e[0m";
// 			if ( nk >= 2 ) s+= "\e[1;33m(\e[0m";
// 			for(int k=0; k<nk; ++k) {
// 				if (k != 0) s += delim;
// 				s += ght[ i->options[j][k] ];
// 				}
// 			if ( nk >= 2 ) s+= "\e[1;33m)\e[0m";
// 			}
// 		if (do_paren) s += "\e[1;33m)\e[0m";
// 		if (do_qmark) s += "\e[1;33m?\e[0m";
// 		}
// // 	printf("%s\n", s.c_str());
// 	return s;
// 	}


string regex::to_str(const char* delim, const char* lrb, const char* option_sep, const char* rrb, const char* qmark) const {
	string s;
	for(const_iterator i=begin(); i!=end(); ++i) {
		bool do_paren = 0;
		bool do_qmark = 0;
		int on = i->options.size();
		if ( i != begin() ) s += delim;
		if ( on >= 2 ) {
			do_paren = 1;
			for(unsigned int j=0; j < i->options.size(); ++j) {
				if ( ( i->options[j].size() == 0 ) ) {
					if ( on == 2 ) do_paren = 0;
					do_qmark = 1;
					}
				}
			}
		if (do_paren) s += lrb;
		int c=0;
		for(int j=0; j<on ; ++j) {
			int nk = i->options[j].size() ;
			if ( nk == 0 ) continue;
			if ( c != 0 ) s += option_sep;
			if ( nk >= 2 ) s+= lrb;
			for(int k=0; k<nk; ++k) {
				if (k != 0) s += delim;
				s += escape_str( ght[ i->options[j][k] ] );
				}
			if ( nk >= 2 ) s+= rrb;
			++c;
			}
		if (do_paren) s += rrb;
		if (do_qmark) s += qmark;
		}
// 	printf("%s\n", s.c_str());
	return s;
	}


#define REGEX_DELIM_REGEX_OPEN	"<regex>"
#define REGEX_DELIM_REGEX_CLOSE	"</regex>"
#define REGEX_DELIM_ELEM_OPEN	"<elem>"
#define REGEX_DELIM_ELEM_CLOSE	"</elem>"
#define REGEX_DELIM_GROUP_OPEN	"<group>"
#define REGEX_DELIM_GROUP_CLOSE	"</group>"
#define REGEX_DELIM_OPT_OPEN	"<opt>"
#define REGEX_DELIM_OPT_CLOSE	"</opt>"

string regex::to_xml() const {
	string s;
	s += REGEX_DELIM_REGEX_OPEN;
	for(const_iterator i=begin(); i!=end(); ++i) {
		s += REGEX_DELIM_ELEM_OPEN;
		int on = i->options.size();
		for(int j=0; j<on ; ++j) {
			s += REGEX_DELIM_GROUP_OPEN;
			int nk = i->options[j].size() ;
			for(int k=0; k<nk; ++k) {
				s += REGEX_DELIM_OPT_OPEN;
				s += ght[ i->options[j][k] ];
				s += REGEX_DELIM_OPT_CLOSE;
				}
			s += REGEX_DELIM_GROUP_CLOSE;
			}
		s += REGEX_DELIM_ELEM_CLOSE;
		}
	s += REGEX_DELIM_REGEX_CLOSE;
// 	printf("%s\n", s.c_str());
	return s;
	}

bool regex::simplify(hash_value  hv_wildcard) {
	if (!hv_wildcard) {
		iterator it=begin();
		while(it!=end()) {
			regex_element& it_re = *it;
			
			it_re.options.uniq();
			
			vector< sequence >&  it_re_options = it_re.options;
			vector< sequence >::iterator  j = it_re_options.begin() ;
			
			while(j != it_re_options.end() ) {
				sequence&  it_re_options_j = *j;
				vector<hash_value>::iterator k=it_re_options_j.begin();
				
				while(k != it_re_options_j.end() ) {
					if ( (*k == hv_wildcard) ) {
// 						fprintf(stderr, "@$@#$@#$\n");
						k = it_re_options_j.erase(k);
						}
					else 
						++k;
					}
				
				++j;
				}
			++it;
			}
		}
	
	sort();
	return true;
	}

#include "parse.h"

char* parse_regex_xml(regex& out_re, hash_table& ht, const char* string) {
	out_re.clear();
	char buf[1048476];
	const char* z = string;
	const char* z0;
	if ( ! (z = parse_string(z, REGEX_DELIM_REGEX_OPEN) ) ) return 0;

	z = parse_skip_whitespaces(z);
	
	while( (z0 = parse_string(z, REGEX_DELIM_ELEM_OPEN) ) ) {
		z = z0;
		
		out_re.push_back( regex_element() );
		
		z = parse_skip_whitespaces(z);
		while( (z0 = parse_string(z, REGEX_DELIM_GROUP_OPEN) ) ) {
			z = z0;
			
			out_re.rbegin()->options.push_back( vector<hash_value>() );
			
			z = parse_skip_whitespaces(z);
			while( (z0 = parse_string(z, REGEX_DELIM_OPT_OPEN) ) ) {
				z = z0;
				z = parse_skip_whitespaces(z);
// 				printf("!");
				z = parse_capture_until_string_z(z, REGEX_DELIM_OPT_CLOSE, buf);
				
				hash_value h = ht( buf );
				out_re.rbegin()->options.rbegin()->push_back( h );
				
// 				
				z = parse_skip_whitespaces(z);
				
// 				printf("[%s] . %s\n", buf, z);
				}
				
			z = parse_skip_whitespaces(z);
			if ( ! (z = parse_string(z, REGEX_DELIM_GROUP_CLOSE) ) ) return 0;
			}
		
		z = parse_skip_whitespaces(z);
		if ( ! (z = parse_string(z, REGEX_DELIM_ELEM_CLOSE) ) ) return 0;
		}
	
	z = parse_skip_whitespaces(z);
	if ( ! (z = parse_string(z, REGEX_DELIM_REGEX_CLOSE) ) ) return 0;
		  
	return (char*)z;
	}


bool regex::is_flat() const {
	vector<regex_element>::const_iterator it;
	for(it=begin(); it!=end(); ++it) {
		if (it->options.size() > 1) return false;
		}
	return true;
	}

void regex::sort() {
	for(auto it=begin(); it!=end(); ++it) it->sort();
	}

int regex::deepsilon(void) { // remove all empty options from the head and tail
	vector<regex_element>::iterator  it;
	vector<regex_element>::reverse_iterator  rit;
	int nremoeved = 0;
	
	for(it=begin(); it!=end(); ++it) {
		if (! it->deepsilon()) break;
		++nremoeved;
		}
	for(rit=rbegin(); rit!=rend(); ++rit) {
		if (! rit->deepsilon()) break;
		++nremoeved;
		}
	return nremoeved;
	}

bool regex::operator == (const regex& r1) const {
	if ( size() != r1.size() ) return false;
	vector<regex_element>::const_iterator j = r1.begin();
	for(vector<regex_element>::const_iterator i=begin(); i!=end(); ++i, ++j) {
		if (! (*i == *j) ) return false;
		}
	return true;
	}

void regex::push_back(hash_value h) { 
	regex_element re; 
	vector<hash_value> vhv;
	vhv.push_back(h);
	re.options.push_back(vhv);
	push_back(re);
	}

void regex::push_back(const vector<hash_value>& vhv) { 
	regex_element re; 
	re.options.push_back(vhv);
	push_back(re);
	}

void regex::push_back(const regex_element& re) { 
	vector<regex_element>::push_back(re); 
	}


	
bool regex::match(const token_string& s, const token_string::const_iterator si, const regex::const_iterator rj) const {
	for (unsigned int k=0; k<rj->options.size(); ++k) {
		bool matched_opt = true;
		
		const vector<hash_value>&  rj_options_k = rj->options[k];
		
		int option_len = rj_options_k.size() ;
		
		for (int l = 0; l<option_len; ++l) {
			
			if ( (si+l) == s.end() ) {  // already at the end of the string 
				matched_opt = false;
				break;
				}
			
			if ( f_use_wildcard && (rj_options_k[l] == hv_wildcard) )  continue;
				
			if (f_deep_cmp) {
				if ( ! (si+l)->has_class( rj_options_k[l] ) ) { // the character does not match
					matched_opt = false;
					break;
					}
				}
			else {
				if ( (si+l)->primary() != rj_options_k[l] ) { // the character does not match
					matched_opt = false;
					break;
					}
				}
			}
		
		if (matched_opt) {
			if ( (rj + 1) == end() ) return true;
			if ( match( s, si+option_len, rj+1) )  return true;
			}
		}
	return false;
	}


bool regex::match(const token_string&  ts) const {
	for (token_string::const_iterator i=ts.begin(); i!=ts.end(); ++i) {
		if ( match( ts, i, begin() ) ) return true;
		}
	return false;
	}

	
token_string::const_iterator regex::find(const token_string&  ts, token_string::const_iterator  start) const {
	for (token_string::const_iterator i=start; i!=ts.end(); ++i) {
		if ( match( ts, i, begin() ) ) return i;
		}
	return ts.end();
	}

	
vector<int> regex::find_all(const token_string&  ts) const {
	vector<int>  retval;
	
	token_string::const_iterator s=ts.begin();
	
	for (unsigned int i=0; i<ts.size(); ++i) {
		if ( match( ts, s+i, begin() ) ) retval.push_back(i);
		}
	
	return retval;
	}


double regex_element::atfidf(const sample_list& sl) const { 
	double x = 0;
	for(unsigned int i=0; i<options.size(); ++i) {
		double x0 = 0;
		for(unsigned int j=0; j<options[i].size(); ++j) {
			map<hash_value, double>::const_iterator k = sl.atfidf.find( options[i][j] );
			if ( k != sl.atfidf.end() ) x0 += k->second ;
			}
		x += x0;
		}
	
	if ( options.size() ) x /= double( options.size() );
	return x;
	}
	
double regex::atfidf(const sample_list& sl) const { 
	double x = 0;
	for(const_iterator i=begin(); i!=end(); ++i) {
		x += i->atfidf(sl);
		}
	return x;
	}
