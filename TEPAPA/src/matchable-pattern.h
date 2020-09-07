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

using namespace std;

struct matching_range {
  size_t  from; //
  size_t  to;   // exclusive
  
  matching_range() {}
  matching_range(const token_string& ts) ;
  matching_range(const token_string& ts, const token_string::iterator ifrom) ;
  matching_range(const token_string& ts, const token_string::const_iterator ifrom) ;
  matching_range(const token_string& ts, const token_string::const_iterator ifrom, const token_string::const_iterator ito) ;
  matching_range(size_t pfrom, size_t pto) { from=pfrom; to=pto; }
};


typedef vector<matching_range>  matching_range_list;

typedef pair<token_string::const_iterator, token_string::const_iterator>  matched_pos; // result of "find" function (matched position)


class matchable_pattern: virtual public pattern {
 public:
 matchable_pattern(): pattern () { }
  
  virtual ~matchable_pattern() {}
  
  // finding the pattern within token string ts from "start" to "end"
  virtual matched_pos find(const token_string&  ts, const matching_range& r) const =0;
  
  // finding the pattern within token string ts from "ts.start" to "ts.end"
  virtual matched_pos find(const token_string&  ts) const ;
  
  // finding all pattersn from within matching range
  virtual vector<size_t> find_all(const token_string&  ts, const matching_range& r) const ;
  
  // finding all pattersn from token_string
  virtual vector<size_t> find_all(const token_string&  ts) const ;
  
  // matching
  virtual bool match(const token_string&  ts, const matching_range& r) const ;
  
  // matching (whole token string)
  virtual bool match(const token_string&  ts) const ;

  virtual pattern* clone() const = 0;
  
  // matching (whole token string)
  virtual binary_profile match(const sample_list& sl) const ;

  // matching (whole token string)
  virtual binary_profile match(const sample_list& sl, const matching_range_list& rl) const ;
  
  virtual binary_profile match(const sample_list& sl, const binary_profile& search_mask) const ;

  virtual binary_profile match(const sample_list& sl, const binary_profile& search_mask, const matching_range_list& rl) const ;

  
  virtual bool has_subpat(const matchable_pattern& ssp) const { return false; }
  
  virtual double atfidf(const sample_list& sl) const { return 0.0; }
};


#endif // __matchable_pattern_h  1
