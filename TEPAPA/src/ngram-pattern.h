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
#ifndef __ngram_pattern_h
#define __ngram_pattern_h 1


using namespace std;


#include <string>
#include <vector>

#include <limits.h>
#include <math.h>

#include "hashtable.h"
#include "strfriends.h"

#include "matchable-pattern.h"


using namespace std;


class ngram_pattern: public matchable_pattern, public vector<hash_value> {
	
  bool f_use_wildcard ;
  bool f_deep_cmp;
	
  static hash_value ngram_pattern_hv_type_id;
		
  virtual void set_type_id() {
    if (ngram_pattern_hv_type_id == 0) ngram_pattern_hv_type_id = ght("NGRAM");
  }
	
  void initialise() {
    set_type_id(); 
    f_use_wildcard = false;
    f_deep_cmp = false;
  }
	
 public:
  
  using matchable_pattern::match;
  
 ngram_pattern(): matchable_pattern(), vector<hash_value>() {  initialise(); }
  
 ngram_pattern(int n): matchable_pattern(), vector<hash_value>(n) { initialise(); }
  
 ngram_pattern(const vector<hash_value>& vh): matchable_pattern(), vector<hash_value>(vh) { initialise(); }
  
  ngram_pattern(const string& serialised_str);

  virtual pattern* clone() const {
    ngram_pattern* np = new ngram_pattern;
    for (const_iterator i=begin(); i!=end(); ++i) np->push_back(*i);
    return np;
  }
  
  
  void set_use_wildcard(bool flag=true) { f_use_wildcard = flag; }
  void set_deep_cmp(bool flag=true) { f_deep_cmp = flag; }
  
  int sim(const ngram_pattern& ssp) const ;
  
  virtual bool has_subpat(const matchable_pattern& ssmp) const ;
  
  virtual string to_string(const string& delim="  ") const ;
  
  static hash_value type_id() {return ngram_pattern_hv_type_id;}
  virtual hash_value get_type_id() const { return ngram_pattern_hv_type_id; }
  
  virtual bool is_numeric() const { return false; }
  
  virtual bool match(const token_string& ts, const matching_range& r) const ;
  
  virtual matched_pos find(const token_string&  ts, const matching_range& r) const ;
  
  virtual double atfidf(const sample_list& sl) const ;
  
  virtual string gen_pcre() const ;
  
  // 	virtual string serialise_proper() const;
  // 	virtual size_t unserialise_proper(const string& serialised_str, size_t pos);
};



///////////////////////////////////////////////////////////////////////////

#include "evaluable-pattern.h"
	
class extractable_ngram_pattern: public ngram_pattern, public evaluable_pattern<double> {
	
  static hash_value hv_extractable_ngram_pattern_id;
		
  virtual void set_type_id() {
    if (hv_extractable_ngram_pattern_id == 0) hv_extractable_ngram_pattern_id = ght("EXNGRAM");
  }

 protected:
  virtual bool is_invalid(double v) const {return isnan((double)v); }
  
 public:
  using matchable_pattern::match;
  using evaluable_pattern::eval;
  
  int  tag_i;

 extractable_ngram_pattern(): ngram_pattern(), evaluable_pattern<double>() {  set_type_id(); tag_i = -1; }
	
 extractable_ngram_pattern(int n): ngram_pattern(n), evaluable_pattern<double>()  { set_type_id(); tag_i = -1; }
	
 extractable_ngram_pattern(const vector<hash_value>& vh): ngram_pattern(vh), evaluable_pattern<double>() {
    set_type_id();
    tag_i = -1;
  }	

  virtual pattern* clone() const {
    extractable_ngram_pattern* enp = new extractable_ngram_pattern;
    for (const_iterator i=begin(); i!=end(); ++i) enp->push_back(*i);
    enp->tag_i = tag_i;
    return enp;
  }
  
  static hash_value type_id() {return hv_extractable_ngram_pattern_id;}
  
  virtual hash_value get_type_id() const { return hv_extractable_ngram_pattern_id; }
  
  virtual string to_string() const ;
  
  virtual bool is_numeric() const final { return true; }
  
  virtual double eval(const token_string&  ts, const matching_range& r) const ;
  
  virtual double eval(const token_string&  ts) const {
    return eval(ts, matching_range(ts));
  }
};


#endif //  __ngram_pattern_h
