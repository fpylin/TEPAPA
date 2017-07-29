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
#include "bowsent.h"
#include <algorithm>


string bag_of_words_sentence::to_string(const string& delim) const {
	string s;
	for (const_iterator  i = begin(); i!= end(); ++i) {
		if ( i != begin() ) s += "|";
		s += ght[ *i ];
		}
	return s ;
	}

void bag_of_words_sentence::reset() {
	}

void bag_of_words_sentence::scan(hash_value hv) {
	if ( hv_separators.find(hv) != hv_separators.end() ) {
		clear();
		}
	else {
		insert(hv);
		}
	}


void bag_of_words_sentence::hash(void) { 
	hv_bowsent = ght( to_string().c_str() ); 
	}

bag_of_words_sentence operator & (const bag_of_words_sentence& rs1, const bag_of_words_sentence& rs2) {
	
	bag_of_words_sentence retval;
	
	set_intersection (rs1.begin(), rs1.end(), rs2.begin(), rs2.end(), std::inserter(retval, retval.end()) );
	retval.hash();
	
	return retval;
	}

	
double jaccard (const bag_of_words_sentence& a, const bag_of_words_sentence& b) {
	bag_of_words_sentence a_and_b;
	bag_of_words_sentence a_or_b;
	
	set_intersection (a.begin(), a.end(), b.begin(), b.end(), std::inserter(a_and_b, a_and_b.end()) );
	set_union (a.begin(), a.end(), b.begin(), b.end(), std::inserter(a_or_b, a_or_b.end()) );
	
	double x = double( a_and_b.size() ); 
	double y = double( a_or_b.size() );
	if (y == 0) y += 1;
	
	return x / y;
	}


hash_value bag_of_words_sentence_pattern::bag_of_words_sentence_pattern_hv_type_id = 0;


void bag_of_words_sentence::gen_multiplets(vector<bag_of_words_sentence>& out, const bag_of_words_sentence& stack, int n, const_iterator start) const {
	
	for(const_iterator it=start; it!=end(); ++it) {
		bag_of_words_sentence  new_stack = stack;
		new_stack.insert(*it); 
		if ((n - 1) <= 0) {
// 			printf( "%s\n", new_stack.to_string().c_str() );
			out.push_back(new_stack);
			}
		else {
			const_iterator it1 = it; ++it1;
			gen_multiplets(out, new_stack, n-1, it1);
			}
		}
	}

vector<bag_of_words_sentence> bag_of_words_sentence::gen_multiplets(int n) const {
	vector<bag_of_words_sentence>   out;
	bag_of_words_sentence  stack;
	gen_multiplets( out, stack, n, begin() );
	return out;
	}


vector<bag_of_words_sentence>  bag_of_words_sentence::convert_from(const token_string& ts) {
	vector<bag_of_words_sentence> vrs;

	bag_of_words_sentence  rsi;
	
	for(unsigned int j=0; j<ts.size(); ++j) {
		hash_value hv = ts[j].primary();
		if ( ( hv_separators.find(hv) != hv_separators.end() ) ) {
			rsi.hash();
			vrs.push_back(rsi);
			rsi.clear();
			continue;
			}
		rsi.insert( hv );
		}

	if ( rsi.size() ) {
		rsi.hash();
		vrs.push_back(rsi);
		}
	
	return vrs;
	}


////////////////////////////////////////////////////////////

	
bag_of_words_sentence_index::bag_of_words_sentence_index()
	: vector<position_index> () { 
	}
		
bag_of_words_sentence_index::bag_of_words_sentence_index(const vector< vector<bag_of_words_sentence> >&  vvrs)
	: vector< map<hash_value, vector<int> > > () {
	create(vvrs);
	}
	
void bag_of_words_sentence_index::create(const vector< vector<bag_of_words_sentence> >&  vvrs) {
	
	clear();
	
	for (unsigned int i=0; i<vvrs.size(); ++i) {
		map<hash_value, vector<int> >  index_sample;
		
		for (unsigned int j=0; j<vvrs[i].size(); ++j) {
		for(bag_of_words_sentence::const_iterator k=vvrs[i][j].begin(); k!=vvrs[i][j].end(); ++k) {
				index_sample[*k].push_back(j);
				}
			}
	
		push_back(index_sample);
		}
	}
	
vector<int> bag_of_words_sentence_index::get_positions_by_token_index(int j, const bag_of_words_sentence& bowsent) const {
		
	const position_index& token_index_j =  *(begin() + j);
	
	vector<int> pos ;
	
	for(bag_of_words_sentence::const_iterator it = bowsent.begin(); it != bowsent.end(); ++it) {
		
		map<hash_value, vector<int> >::const_iterator  u = token_index_j.find( *it );
		
		if ( u == token_index_j.end() ) break;
		
		vector<int>   v = u -> second;
		vector<int>   w;
		if ( it == bowsent.begin() ) { 
			pos = v;
			}
		else {
			sort(pos.begin(), pos.end());
			sort(v.begin(), v.end());
			set_intersection(pos.begin(),pos.end(),v.begin(),v.end(),back_inserter(w));
			pos = w;
			}
		if ( ! pos.size() ) break;
		}
	return pos;
	}

binary_profile bag_of_words_sentence::has_pattern(const vector< vector<bag_of_words_sentence> >&  haystack, const bag_of_words_sentence_index& token_index) const {
	binary_profile bp( haystack.size() );
	
	for(unsigned int i=0; i<haystack.size(); ++i) {
		bool f_retval = false;
		
 		vector<int> pos = token_index.get_positions_by_token_index(i, *this);
		
//		for(unsigned int ii=0; ii<haystack[i].size(); ++ii) {
 		for(unsigned int z=0; z<pos.size(); ++z) {
 			unsigned ii = pos[z];
			const bag_of_words_sentence& rsiii = haystack[i][ii];
			bag_of_words_sentence bowsentc = rsiii & *this;
			if ( ! bowsentc.size() ) continue;
			if ( bowsentc == *this ) {
				f_retval = true;
				break;
				}
			}
		bp[i] = f_retval;
		}

	return bp;
	}

double bag_of_words_sentence::calc_dissimilarity_metric(const bag_of_words_sentence& rs1, const bag_of_words_sentence& rs2, int type_dissimilarity_metric) {
// 	int n1 = rs1.size();
// 	int n2 = rs2.size();
// 	int N = n1 + n2;
// 	if (N == 0) ++N;
// 	int d = levenshtein(rs1, rs2);
// 	int d = ;
// 	double f = double(d) / double(N);
// 	printf("%d\t%d\t%g\n", d, N, f);
	return 1.0 - jaccard(rs1, rs2);
	}

vector<bag_of_words_sentence_pattern>  bag_of_words_sentence_pattern::convert_from(const token_string& ts) {
	vector<bag_of_words_sentence> v = bag_of_words_sentence::convert_from(ts);
	vector<bag_of_words_sentence_pattern>  retval;
	for(unsigned int i=0; i<v.size(); ++i)  retval.push_back(v[i]);
	return retval;
	}
	
