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
#ifndef  __pattern_h
#define  __pattern_h  1

#include "iptr.h"
#include "hashtable.h"

class pattern: public __iptr_member {
	protected:
	hash_value  hv_uniq_id;  // uniq ID created from the string presentation
// 	hash_value  hv_type_id;  // uniq presentation

	virtual void set_type_id() =0;
	
	public:
	virtual hash_value get_type_id() const =0;
		
	pattern(): __iptr_member() { hv_uniq_id = 0; /*hv_type_id = p_hv_type_id; */}       
	
	virtual ~pattern() {}
	
	virtual pattern* clone() const =0;       
	
	virtual string to_string(const string& delim="  ") const = 0;
	
	hash_value get_uniq_id() ;
		
	virtual bool simplify(hash_value hv_wildcard) { return false; }
	
	virtual bool is_numeric() const =0;
	
// 	virtual string serialise_proper() const =0;
	
// 	virtual string serialise() const =0;
	
	virtual string gen_pcre() const { return ""; }
	
	// 	static iptr<pattern>  unserialise(const string& str) ;
	};


#endif // __pattern_h
