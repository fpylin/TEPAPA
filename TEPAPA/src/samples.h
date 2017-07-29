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
#ifndef __samples_h
#define __samples_h 1

#include "hashtable.h"
#include "tepapa.h"

#include "iptr.h"
#include "token.h"
#include "bprof.h"
// #include "regex.h"

#include <assert.h>
#include <stdio.h>

struct sample {
	enum datatype {numeric_t=1, symbolic_t=2, symbol_list_t=3, null=0} dtype;
	hash_value           symbol;
	int                  sample_order;
	double               score;
	vector<hash_value>   symbol_list; // with symbol_list_t;
	
	vector<hash_value>   definitions;
	token_string         data;
	
	sample(datatype pdtype=numeric_t){ dtype=pdtype; }
	
	int load_definitions() ;
	int load_charstring(const string& s) ; // treat each character as token
	
	bool save(FILE* fout=stdout) const;
	};
	

#define SAMPLE_LIST_OUTVAR_AUTO         0
#define SAMPLE_LIST_OUTVAR_BINARY       1
#define SAMPLE_LIST_OUTVAR_NUMERIC      2

class sample_list: public vector<sample> {
	int                     outvar_type;
	
	public:
	hash_value              sample_list_name;
	
	bool is_outvar_binary()  const ;
	bool is_outvar_numeric() const ;
	void guess_outvar_type();
	
	hash_assoc_array< binary_profile >  token_registry;
	map<hash_value, double> atf;     // average term (token) frequency
	map<hash_value, double> df;      // document frequency
	map<hash_value, double> atfidf;  // average tf-idf
	
	bool load(const string& filename, sample::datatype pdtype=sample::numeric_t) ;
	bool save(const string& filename) ;

	bool load_charstrings(const string& filename) ;
	
	sample_list() ;
	sample_list(const sample_list& sl);
	sample_list(const string& filename, sample::datatype pdtype=sample::numeric_t) ;
	
	int load_all_definitions() ;
	void index_token_registry() ;
	
	binary_profile gold_standard() const ;
	
	void sort(); 
	void shuffle(); 
	
	vector<double>  get_scores() const ;
	
	bool summarise(map<hash_value, double>& OUT_statistics) const;
	void reorder(const vector<int>& o);
	
	void reduce_token_sets(void);
	};

#endif // __sample_h
