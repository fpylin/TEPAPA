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
#include "samples.h"
#include "parse.h"
#include "stats.h"
#include <stdio.h>
#include "tepapa-msg.h"
#include <algorithm>
#include <random>



int sample::load_definitions() {
	const char* filename = ght[ definitions[0] ];
	msgf(VL_INFO, "Loading %s\n", filename);
	data.load_from_file(filename);

	return data.size();
	}

int sample::load_charstring(const string& s) {
	msgf(VL_INFO, "Loading string %s\n", s.c_str());
	data.load_charstring(s);
	return data.size();
	}
	
bool sample::save(FILE* fout) const {
// 	if(!data) return false;
	
	for(unsigned int i=0; i<data.size(); ++i) {
		const token& t = data[i];
		for(unsigned int j=0; j<t.size(); ++j) {
			if (j) fprintf(fout, "\t");
			else fprintf(fout, "[%lu]", t.size());
			fprintf(fout, "%s", ght[ t[j] ]);
			}
		fprintf(fout, "\n");
		}
	return true;
	}


sample_list::sample_list() : vector<sample>(){ 
	outvar_type = SAMPLE_LIST_OUTVAR_AUTO;
	}

sample_list::sample_list(const sample_list& sl): vector<sample>(sl) {
	outvar_type = SAMPLE_LIST_OUTVAR_AUTO;
	}

	
sample_list::sample_list(const string& filename, sample::datatype pdtype)
	: vector<sample>() {
	outvar_type = SAMPLE_LIST_OUTVAR_AUTO;
	load(filename, pdtype);
	}


bool sample_list::load(const string& filename, sample::datatype pdtype) {
	char buf[4096];
	FILE* fin = fopen(filename.c_str() ,"rt");
	
	if (!fin) {
		msgf(VL_FATAL, "Unable to open file %s", filename.c_str() );
		}
	
	clear();
	
	while( fgets(buf, sizeof(buf), fin) ) {
		char* x = buf;
		double d = 0;
		
		sample s(pdtype);

		char symb[65536];
		switch(pdtype) {
			case sample::numeric_t:
				if ( ! ( x = parse_double(x, d) ) ) continue;
				s.score = d;
				break;
			case sample::symbolic_t:
				if ( ! ( x = parse_capture_until_char(x, '\t', symb) ) ) continue;
				s.symbol = ght( symb );
				break;
			case sample::symbol_list_t:
				s.symbol_list.clear();
				while(x) {
					if ( ! strchr(x+1, '\t') ) break;
					x = parse_capture_until_char(x, '\t', symb);
					hash_value h = ght( symb ) ;
					s.symbol_list.push_back( h );
					if ( ! ( x = parse_skip_char(x, '\t') ) ) continue;
					};
				break;
			default: ;
			};
		
		char filename[1024];
		if ( ! ( x = parse_skip_char(x, '\t') ) ) continue;
		if ( ! ( x = parse_capture_until_chars(x, "\n\t", filename) ) ) continue;
		
		s.definitions.push_back( ght(filename) );
		s.sample_order = size();
		push_back(s);
		
		switch (pdtype) {
			case sample::numeric_t: 
				msgf(VL_DEBUG, "%f\t%lu\t%s\n", d, s.definitions[0], filename);
				break;
			case sample::symbolic_t: 
				msgf(VL_DEBUG, "%s\t%lu\t%s\n", ght[ s.symbol ], s.definitions[0], filename);
				break;
			case sample::symbol_list_t:
				for(unsigned int z=0; z<s.symbol_list.size(); ++z)  msgf(VL_DEBUG, "%s\t", ght[ s.symbol_list[z] ]);
				msgf(VL_DEBUG, "%lu\t%s\n", s.definitions[0], filename);
				break;
			default:;
			};
		}
	
	fclose(fin);
	
	return 1;
	}


bool sample_list::load_charstrings(const string& filename) {
	char buf[1048576];
	FILE* fin = fopen(filename.c_str() ,"rt");
	
	if (!fin)  msgf(VL_FATAL, "Unable to open file %s", filename.c_str() );
	
	clear();
	while( fgets(buf, sizeof(buf), fin) ) {
		char* x = buf;
		char* y =0;
		double d = 0;

		if ( ! ( x = parse_double(x, d) ) ) continue;
		if ( ! ( x = parse_skip_whitespaces(x) ) ) continue;
		if ( (y = strchr(x, '\n') ) ) *y = 0;
		if ( (y = strchr(x, '\r') ) ) *y = 0;
		
		sample s;
		s.score = d;
		s.sample_order = size();
		s.load_charstring( x );
		s.definitions.push_back( ght(x) );
		push_back(s);
		}
	
	fclose(fin);

	index_token_registry();
	
	return 1;
	}

	
bool sample_list::save(const string& filename) {
	FILE* fout = fopen( filename.c_str(), "wt");
	
	if ( !fout ) msgf(VL_FATAL, "Unable to write to %s", filename.c_str() );
	
	for(iterator it=begin(); it!=end(); ++it) {
		fprintf(fout, "%f\t%s\n", it->score, ght[ it->definitions[0] ] );
		}
	
	fclose(fout);
	
	return 1;
	}

	
	

int sample_list::load_all_definitions() {
	msgf(VL_INFO, "Loading all definitions.\n");
	for(iterator i=begin(); i!=end(); ++i) {
		i->load_definitions();
		}
		
	reduce_token_sets();
	
	index_token_registry();
	
// 	int sum=0;
// 	for(iterator i=begin(); i!=end(); ++i) {
// 		printf( "%d\t%d\n", i-begin(), i->data.size() );
// 		sum+= i->data.size() ;	
// 		}
// 	printf( "%d\n", sum );
	
	
	return 1;
	}

void sample_list::index_token_registry() {
	msgf(VL_INFO, "Create index of binary profiles.\n");
	token_registry.set_hash_table(ght);
	
	int ii=0;
	
	for(const_iterator i=begin(); i!=end(); ++i, ++ii) {
// 		msgf(VL_INFO, "Indexing binary profiles. %d\r", ii);
		const token_string& ts = (i->data);
		
		for(token_string::const_iterator j=ts.begin(); j!=ts.end(); ++j) {
			const token& t = *j;
			if ( ! t.size() ) continue;
			if ( hv_separators.find( t.primary() ) != hv_separators.end() )  continue;
			for(token::const_iterator k=t.begin(); k!=t.end(); ++k) {
				hash_value hv = *k;
				if ( token_registry[hv].size() != size() ) token_registry[hv].resize( size() );
				token_registry[hv][ii] = true;
				}
			}
		}

// 	msgf(VL_INFO, "Indexing binary profiles done.\n");
// 	
// 	for(int i=0;i<ght.size(); ++i) {
// 		printf("%4d  %30s  %s\n", i, ght[i], (token_registry[i] ? token_registry[i]->digest().c_str() : "null"));
// 		}

	for(const_iterator i=begin(); i!=end(); ++i, ++ii) {
		const token_string& ts = (i->data);
		map<hash_value, double> tf;
		
		for(token_string::const_iterator j=ts.begin(); j!=ts.end(); ++j) {
			const token& t = *j;
			
			if ( ! t.size() ) continue;
			if (hv_separators.find( t.primary() ) != hv_separators.end() ) continue;
			
			for(token::const_iterator k=t.begin(); k!=t.end(); ++k) {
				hash_value hv = *k;
				tf[hv] += 1;
				}
			}
		
		for (map<hash_value, double>::const_iterator z=tf.begin(); z!=tf.end(); ++z) {
			df[ z->first ] += 1;
			atf[ z->first ] += z->second;
			}
		}

	// calculating atfidf
	
	double n = double( size() );
	for (map<hash_value, double>::const_iterator z=df.begin(); z!=df.end(); ++z) {
		hash_value hv = z->first ;
		atfidf[ hv ] = - log( ( df[ hv ] + 1) / n );
		atf[ hv ] /= n;
		df[ hv ] /= n;
		}
	}


binary_profile  sample_list::gold_standard() const {
	binary_profile  retval = size();
	
	for(unsigned int i=0; i<retval.size(); ++i) {
		retval[i] = ( (begin() + i) -> score == 0 ? false : true );
		}
	return retval;
	}

	
bool cmp_score_desc(const sample& s1, const sample& s2) {
	if ( s1.score > s2.score ) return true;
	if ( s1.score < s2.score ) return false;
	return ( s1.sample_order < s2.sample_order );
	}

void sample_list::sort() {
	std::sort (begin(), end(), cmp_score_desc);
	}

void sample_list::shuffle() {
    std::random_device rd;
    std::mt19937 g(rd());

	std::shuffle (begin(), end(), g);
	}

	
bool sample_list::summarise(map<hash_value, double>& OUT_statistics) const {
	OUT_statistics[ ght("Cases") ] = size();

	set<hash_value>  tokens;
	set<hash_value>  tokens_primary;
	set<hash_value>  tokens_auxillary;
	
	int total_bottomost_tokens = 0;
	
	for(const_iterator i=begin(); i!=end(); ++i) {
		const token_string& ts = i->data;
		
		total_bottomost_tokens += i->data.size();
		
		for(token_string::const_iterator j=ts.begin(); j!=ts.end(); ++j) {
			const token& t = (*j);
			
			if ( ! t.size() ) continue;
			
			tokens_primary.insert( t.primary() );
			
			for(token::const_iterator k=t.begin(); k!=t.end(); ++k) {
				hash_value hv = *k;
				tokens.insert(hv);
				if ( k == t.begin() ) continue;
				tokens_auxillary.insert(hv);
				}
			}
		}
	
	OUT_statistics[ ght("Unique primary tokens") ]    = double( tokens_primary.size() );
	OUT_statistics[ ght("Unique auxillary tokens") ]  = double( tokens_auxillary.size() );
	OUT_statistics[ ght("Unique tokens") ]            = double( tokens.size() );
	OUT_statistics[ ght("Total length") ]             = double( total_bottomost_tokens );
	OUT_statistics[ ght("Avg length per case") ]      = double( total_bottomost_tokens ) / double( size() );
		
	return true;
	}

	
bool sample_list::is_outvar_binary()  const { 
	assert (outvar_type != SAMPLE_LIST_OUTVAR_AUTO);
	return (outvar_type == SAMPLE_LIST_OUTVAR_BINARY);
	}

bool sample_list::is_outvar_numeric() const { 
	assert (outvar_type != SAMPLE_LIST_OUTVAR_AUTO);
	return (outvar_type == SAMPLE_LIST_OUTVAR_NUMERIC); 
	}

void sample_list::guess_outvar_type() {
	if (outvar_type != SAMPLE_LIST_OUTVAR_AUTO) return ; // already set
	vector<double> lvls = get_levels( get_scores() );
	if ( lvls.size() == 2 ) {
		msgf(VL_INFO, "Sample is BINARY\n");
		outvar_type = SAMPLE_LIST_OUTVAR_BINARY;
		}
	else {
		msgf(VL_INFO, "Sample is NUMERIC\n");
		outvar_type = SAMPLE_LIST_OUTVAR_NUMERIC;
		}
	}

vector<double>  sample_list::get_scores() const {
	vector<double> retval;
	for(const_iterator it=begin(); it!=end(); ++it) {
		retval.push_back( it->score );
		}
	return retval;
	}

void sample_list::reorder(const vector<int>& o) {
	size_t  sz = o.size();
	assert( sz == size() );
	
	sample_list new_sl = *this; 
	
	for(unsigned int i=0; i<o.size(); ++i) {
		*( begin() + o[i] ) = new_sl[i];
		}
	}


void sample_list::reduce_token_sets(void) {
	
	map< hash_value, set<hash_value> >  set_members;
	
	for(const_iterator it=begin(); it!=end(); ++it) {
		for(token_string::const_iterator jt=it->data.begin(); jt!=it->data.end(); ++jt) {
			const token& t = *jt; 
			
			hash_value k_pri= t.primary();
			for(unsigned int k=1; k<t.size(); ++k) {
				set_members[ t[k] ].insert(k_pri);
				}
			}
		}
	
	unsigned int n_sets_before = set_members.size();
	for (auto i=set_members.begin(); i != set_members.end(); ++i) {
// 		printf("OLD\t%d\t%s\t%d\t", i->first, ght[ i->first ], i->second.size() );
// 		for(auto j=i->second.begin(); j!= i->second.end(); ++j) printf(" [%s]", ght[ *j ] );
// 		printf("\n");
		}
	
	for(iterator it=begin(); it!=end(); ++it) {
		for(token_string::iterator jt=it->data.begin(); jt!=it->data.end(); ++jt) {
			token& t = *jt; 
			for(unsigned int k=t.size()-1; k>=1; --k) {
				if ( set_members[ t[k] ].size() <= 1 ) t.erase( t.begin() + k );
				}
			}
		}
		
	set_members.clear();

	
	for(const_iterator it=begin(); it!=end(); ++it) {
		for(token_string::const_iterator jt=it->data.begin(); jt!=it->data.end(); ++jt) {
			const token& t = *jt; 
			hash_value k_pri= t.primary();
			for(unsigned int k=1; k<t.size(); ++k) {
				set_members[ t[k] ].insert(k_pri);
				}
			}
		}
	unsigned int n_sets_after_1 = set_members.size();
	
	map< set<hash_value>, set<hash_value> >  set_member_sets;
	
	for (auto i=set_members.begin(); i != set_members.end(); ++i) {
// 		printf("NEW\t%d\t%s\t%d\t", i->first, ght[ i->first ], i->second.size() );
// 		for(auto j=i->second.begin(); j!= i->second.end(); ++j) printf(" [%s]", ght[ *j ] );
		set_member_sets[ i->second ].insert( i->first );
// 		printf("\n");
		}
		
	unsigned int n_sets_after_2 = set_member_sets.size();
	
	map< hash_value, hash_value >  replacement_map;
	for (auto i=set_member_sets.begin(); i != set_member_sets.end(); ++i) {
// 		printf("SMS\t");
// 		for(auto j=i->first.begin();  j!= i->first.end();  ++j) printf(" [%s]", ght[ *j ] );
// 		printf("\t -> \t");
// 		for(auto j=i->second.begin(); j!= i->second.end(); ++j) printf(" [%s]", ght[ *j ] );
// 		printf("\n");
		if (i->second.size() <= 1) continue;
		string csym;
		
		for(auto j=i->second.begin(); j!= i->second.end(); ++j) {
			string ss = ght[ *j ];
			if ( ss[0] == '<') ss = ss.substr(1);
			if ( ss[ss.length()-1] == '>') ss = ss.substr(0, ss.length()-1);
			if ( ss[ss.length()-1] == '/') ss = ss.substr(0, ss.length()-1);
			if ( csym.length() ) csym += "," ;
			csym += ss ;
			}
		csym = "<" + csym + "/>";
		
		for(auto j=i->second.begin(); j!= i->second.end(); ++j) {
			replacement_map[ *j ] = ght( csym.c_str() );
			}
		}

	for(iterator it=begin(); it!=end(); ++it) {
		for(token_string::iterator jt=it->data.begin(); jt!=it->data.end(); ++jt) {
			token& t = *jt; 
			for(unsigned int k=t.size()-1; k>=1; --k) {
				if ( replacement_map.find( t[k] ) != replacement_map.end() ) {
					t[k] = replacement_map[ t[k] ];
// 					printf("!\n");
					}
				}
			}
		}

	set_members.clear();
	
	for(const_iterator it=begin(); it!=end(); ++it) {
		for(token_string::const_iterator jt=it->data.begin(); jt!=it->data.end(); ++jt) {
			const token& t = *jt; 
			
			hash_value k_pri= t.primary();
			for(unsigned int k=1; k<t.size(); ++k) {
				set_members[ t[k] ].insert(k_pri);
				}
			}
		}
	
// 	for (auto i=set_members.begin(); i != set_members.end(); ++i) {
// 		printf("FINAL\t%d\t%s\t%d\t", i->first, ght[ i->first ], i->second.size() );
// 		for(auto j=i->second.begin(); j!= i->second.end(); ++j) printf(" [%s]", ght[ *j ] );
// 		printf("\n");
// 		}

	msgf(VL_INFO, "reduce_token_sets(): Reduced %u -> %u -> %u sets\n", n_sets_before, n_sets_after_1, n_sets_after_2);
	}
