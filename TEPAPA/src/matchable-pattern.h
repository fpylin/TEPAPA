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
#ifndef __matchable_pattern_h 
#define __matchable_pattern_h  1

#include "pattern.h"
#include "token.h"
#include "samples.h"

class matchable_pattern: virtual public pattern {
	public:
	matchable_pattern(): pattern () { }
	
	virtual ~matchable_pattern() {}
	
	virtual token_string::const_iterator find(const token_string&  ts, token_string::const_iterator  start) const =0;
	
	virtual token_string::const_iterator find(const token_string&  ts) const ;
	
	virtual vector<int> find_all(const token_string&  ts) const ;
	
	virtual bool match(const token_string&  ts) const ;
	
	virtual binary_profile match(const sample_list& sl) const ;
	
	virtual binary_profile match(const sample_list& sl, const binary_profile& search_mask) const ;
	
	virtual bool has_subpat(const matchable_pattern& ssp) const { return false; }
	
	virtual double atfidf(const sample_list& sl) const { return 0.0; }
	};


#endif // __matchable_pattern_h  1
