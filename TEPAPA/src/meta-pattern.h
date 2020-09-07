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
#ifndef __meta_pattern_h
#define __meta_pattern_h 1 

#include <string>
#include <vector>

#include <limits.h>

#include "hashtable.h"
#include "strfriends.h"

#include "matchable-pattern.h"

class meta_pattern: public matchable_pattern, public vector< iptr<pattern> > {
	
  hash_value    hv_op;
	
 protected:
  static hash_value meta_pattern_hv_type_id;
	
		
  virtual void set_type_id() {
    if (meta_pattern_hv_type_id == 0) meta_pattern_hv_type_id = ght("META");
  }
  
  void initialise() {
    set_type_id(); 
  }
  
 public:
  
  using matchable_pattern::match;
  
 meta_pattern(hash_value p_hv_op): matchable_pattern(), vector< iptr<pattern> >() {
    hv_op = p_hv_op;
    initialise();
  }
  
 meta_pattern(hash_value p_hv_op, int n): matchable_pattern(), vector< iptr<pattern> >(n) { 
    hv_op = p_hv_op;
    initialise(); 
  }
  
 meta_pattern(hash_value p_hv_op, const vector< iptr<pattern> >& vp): matchable_pattern(), vector< iptr<pattern> >(vp) { 
    hv_op = p_hv_op;
    initialise(); 
  }

  virtual pattern* clone() const {
    meta_pattern* mp = new meta_pattern(hv_op);
    for(unsigned int i=0; i<size(); ++i) {
      pattern* pptr = (*(begin()+i))->clone();
      iptr<pattern> ppatt = pptr;
      mp->push_back( ppatt );
    }
    return mp;
  }
  
  // 	meta_pattern(const string& serialised_str);
  
  virtual string to_string(const string& delim) const ;
  
  static hash_value type_id() { return meta_pattern_hv_type_id; }
  virtual hash_value get_type_id() const { return meta_pattern_hv_type_id; }
  
  virtual matched_pos find(const token_string&  ts, const matching_range& r) const {
    // NOT A SINGLE POSITION, THUS FAIL.
    return matched_pos( ts.end(), ts.end() );
  }
  
  virtual bool is_numeric() const { return false; }
  
  virtual bool match(const token_string&  ts, const matching_range& r) const ;
  
  virtual double atfidf(const sample_list& sl) const ;
};

#endif // __meta_pattern_h 1 
