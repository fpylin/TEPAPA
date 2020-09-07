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
#ifndef __predicate_pattern_h
#define __predicate_pattern_h 1

#include "ngram-pattern.h"
#include "evaluable-pattern.h"
#include "hashtable.h"
#include "tepapa.h"
#include <string>

class predicate_pattern: virtual public ngram_pattern, public evaluable_pattern<double> {
	static hash_value  predicate_pattern_hv_type_id;
	
	hash_value  hv_func;
	hash_value  hv_type_id;
	
	virtual void set_type_id() {
// 		if (predicate_pattern_hv_type_id == 0) predicate_pattern_hv_type_id = ght("PREDICATE");
		string type_name = string("PREDICATE.") + ght[hv_func];
		hv_type_id = ght( type_name.c_str() );
		}
	
	virtual double eval(const token_string&  ts) const ;
	
	public:
	using matchable_pattern::match;
	using evaluable_pattern::eval;
	
	predicate_pattern(hash_value p_hv_func) : ngram_pattern() { 
		hv_func = p_hv_func;
		set_type_id();
		}

	predicate_pattern(const ngram_pattern& ngpatt, hash_value p_hv_func) 
		: ngram_pattern(ngpatt) {
		hv_func = p_hv_func; 
		set_type_id();
		}
// 	virtual string to_string() const ;
	static map<hash_value, double(*)(const ngram_pattern&, const token_string&)>  func_table;

	static void initialise();

	virtual pattern* clone() const {
	  predicate_pattern* prp = new predicate_pattern(hv_func);
	  prp->hv_type_id = hv_type_id;
	  for (const_iterator i=begin(); i!=end(); ++i) prp->push_back(*i);
	  return prp;
	}
	
	virtual bool is_numeric() const final { return true; }
	virtual bool is_invalid(double v) const { return std::isnan(v); }
	virtual hash_value get_type_id() const { return hv_type_id; }
	};


#include "tepapa-program.h"

class TEPAPA_Program_Predicate_Discoverer: public TEPAPA_Program {
	string func_name ;
	
	public:
	TEPAPA_Program_Predicate_Discoverer();
	
	virtual int run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) ;
	
	virtual TEPAPA_Program* spawn() const { return new TEPAPA_Program_Predicate_Discoverer; }
	};


#endif // __predicate_pattern_h 1
