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
#ifndef __bowsents_h
#define __bowsents_h 1

///////////////////////////////////////////////////////////////////////////

#include <set>
#include <map>
#include "token.h"
#include "hashtable.h"
#include "bprof.h"

extern set<hash_value> hv_separators;

using namespace std;

class bag_of_words_sentence: public set<hash_value>  {
	hash_value  hv_bowsent ; 
	
	public:
	bag_of_words_sentence(): set<hash_value>() { hv_bowsent =0;}
	void reset() ;
	void scan(hash_value hv) ;
	virtual string to_string(const string& delim="  ") const;
	
	bool has(hash_value h) const {return find(h) != end(); }
	bool has(hash_value h, hash_value k) const {return find(h) != end(); }
	
	void hash(void) ;
	
// 	bool operator< (const bag_of_words_sentence& bs) const { return hv_bowsent < bs.hv_bowsent; }
	
	void gen_multiplets(vector<bag_of_words_sentence>& out, const bag_of_words_sentence& stack, int n, const_iterator start) const;
	
	binary_profile has_pattern(const vector< vector<bag_of_words_sentence> >&  haystack, const class bag_of_words_sentence_index& token_index) const ;
	
	vector<bag_of_words_sentence> gen_multiplets(int n) const;

	static double calc_dissimilarity_metric(const bag_of_words_sentence& rs1, const bag_of_words_sentence& rs2, int type_dissimilarity_metric=0);
	
	friend bag_of_words_sentence operator & (const bag_of_words_sentence& rs1, const bag_of_words_sentence& rs2) ;
	friend double jaccard (const bag_of_words_sentence& a, const bag_of_words_sentence& b);
	
	static vector<bag_of_words_sentence>  convert_from(const token_string& ts) ;
	};


typedef map<hash_value, vector<int> >  position_index;

struct bag_of_words_sentence_index: public vector<position_index> {
	
	public:
	bag_of_words_sentence_index() ;

	bag_of_words_sentence_index(const vector< vector<bag_of_words_sentence> >&  vvrs) ; 
	
	void create(const vector< vector<bag_of_words_sentence> >&  vvrs) ;
	
	vector<int> get_positions_by_token_index(int j, const bag_of_words_sentence& bowsent) const ;
	};



#include "pattern.h"

class bag_of_words_sentence_pattern: public pattern, public bag_of_words_sentence {
	static hash_value bag_of_words_sentence_pattern_hv_type_id;
			
	virtual void set_type_id() {
		if (bag_of_words_sentence_pattern_hv_type_id == 0) bag_of_words_sentence_pattern_hv_type_id = ght("BOWSENT");
// 		hv_type_id = bag_of_words_sentence_pattern_hv_type_id ;
		}
		
	public:
	bag_of_words_sentence_pattern(): pattern(), bag_of_words_sentence() { set_type_id(); }
	
	bag_of_words_sentence_pattern(const bag_of_words_sentence& rs): 
		pattern(), bag_of_words_sentence(rs) { set_type_id(); }
	
	virtual ~bag_of_words_sentence_pattern() {}
	virtual string to_string(const string& delim="  ") const { return bag_of_words_sentence::to_string(delim); }
	
	static hash_value type_id () { return bag_of_words_sentence_pattern_hv_type_id; }
	virtual hash_value get_type_id () const { return bag_of_words_sentence_pattern_hv_type_id; }
	
	static vector<bag_of_words_sentence_pattern>  convert_from(const token_string& ts) ;
	
	virtual bool is_numeric() const final { return false; }
	};
	
#endif // __bowsents_h

