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
#ifndef __gseq_pattern_h
#define __gseq_pattern_h 1

using namespace std;


#include <string>
#include <vector>

#include <limits.h>
#include <math.h>

#include "hashtable.h"
#include "strfriends.h"
#include "matchable-pattern.h"
#include "ngram-pattern.h"

/*
 * Gapped sequence is defined as a series of NGRAMs separated by gaps such that:
 * GSEQ := NGRAM1 .* NGRAM2 .* NGRAM3
 */

class gapped_sequence_pattern: public matchable_pattern, public vector<ngram_pattern> {
	
  static hash_value gapped_sequence_pattern_hv_type_id;
		
  virtual void set_type_id() {
    if (gapped_sequence_pattern_hv_type_id == 0) gapped_sequence_pattern_hv_type_id = ght("GSEQ");
  }
  
  void initialise() {
    set_type_id(); 
  }
  
 public:
  using matchable_pattern::match;
  
 gapped_sequence_pattern(): matchable_pattern(), vector<ngram_pattern>() {  initialise(); }
  
 gapped_sequence_pattern(int n): matchable_pattern(), vector<ngram_pattern>(n) { initialise(); }
  
 gapped_sequence_pattern(const vector<ngram_pattern>& vh): matchable_pattern(), vector<ngram_pattern>(vh) { initialise(); }
  
  gapped_sequence_pattern(const string& serialised_str);

  virtual pattern* clone() const { return new gapped_sequence_pattern(*this); }
  
  virtual bool has_subpat(const matchable_pattern& ssmp) const ;
  
  virtual string to_string(const string& delim="  ") const ;
  
  static hash_value type_id() {return gapped_sequence_pattern_hv_type_id;}
  virtual hash_value get_type_id() const { return gapped_sequence_pattern_hv_type_id; }
  
  virtual bool is_numeric() const { return false; }
  
  virtual bool match(const token_string&  ts) const ;
  
  virtual matched_pos find(const token_string&  ts, const matching_range& r) const ;
  
  virtual matched_pos find(const token_string&  ts) const {
    return find( ts, matching_range(ts) );
  }
  
  virtual vector<size_t> find_all(const class token_string&  ts, const matching_range& r) const ;
  
  virtual double atfidf(const sample_list& sl) const ;
  
  virtual string gen_pcre() const ;
};


#endif // __gseq_pattern_h
