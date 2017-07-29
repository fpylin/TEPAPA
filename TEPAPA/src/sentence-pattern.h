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

#ifndef __sentence_pattern_h
#define __sentence_pattern_h 1

#include <map>
#include "token.h"
#include "hashtable.h"
#include "bprof.h"
#include "ngram-pattern.h"
#include "strmetric.h"

extern set<hash_value> hv_separators;

using namespace std;


class sentence_pattern: public ngram_pattern {
	static hash_value sentence_pattern_hv_type_id;
			
	virtual void set_type_id() {
		if (sentence_pattern_hv_type_id == 0) sentence_pattern_hv_type_id = ght("SENPATT");
// 		hv_type_id = sentence_pattern_hv_type_id ;
		}
		
	public:
	sentence_pattern(): ngram_pattern() { set_type_id(); }
	
	virtual ~sentence_pattern() {}
	
	static vector<sentence_pattern>  convert_from(const token_string& ts) ;
	
	static double calc_dissimilarity_metric(const sentence_pattern& rs1, const sentence_pattern& rs2, int type_dissimilarity_metric=0);
	virtual bool is_numeric() const final { return true; }
	
	virtual hash_value get_type_id() const { return sentence_pattern_hv_type_id; }
	};
	


#endif // __sentence_pattern_h
