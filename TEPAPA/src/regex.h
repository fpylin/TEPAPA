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
#ifndef __regex_h
#define __regex_h 1

#include "hashtable.h"
#include "strfriends.h"
#include "matchable-pattern.h"

#include <vector>
#include <string>

using namespace std;

////////////////////////////////////////////////////////////////////////////////////////
struct sequence: public vector<hash_value> {
 sequence(): vector<hash_value> () {}
 sequence(const vector<hash_value>& v): vector<hash_value> (v) {}
  std::string to_str() const ;
};

////////////////////////////////////////////////////////////////////////////////////////
struct sequences: public vector<sequence> {
 public:
  map<hash_value, int> count_tokens() const;
  int split_by(hash_value hv_sep, sequences& out_group1, sequences& out_group2) const;
  std::string to_str(const string& prefix="") const ;
  void push_back_unique(const sequence& s);
  void uniq();
};


////////////////////////////////////////////////////////////////////////////////////////
struct regex_element {
  // 	vector< vector<hash_value> > options;
  sequences  options; // options is composed of a list of sequences
  
  double atfidf(const sample_list& sl) const;
  
  bool operator == (const regex_element& re1) const {
    if ( options.size() != re1.options.size() ) return false;
    for(unsigned int i=0; i<options.size(); ++i) {
      if ( options[i].size() != re1.options[i].size() ) return false;
      for(unsigned int j=0; j<options[i].size(); ++j) {
	if ( options[i][j] != options[i][j] ) return false;
      }
    }
    return true;
  }
  
  int deepsilon(void) ; // remove all empty options
  void sort();
};


////////////////////////////////////////////////////////////////////////////////////////
class regex: public matchable_pattern, public vector<regex_element> {
	
  static hash_value regex_pattern_hv_type_id;
	
  bool f_use_wildcard ;
  bool f_deep_cmp;

  // si_end is the output variable of the end of matched string 
  bool match(const token_string& s, const token_string::const_iterator si, const regex::const_iterator rj, token_string::const_iterator* si_end=0) const;
	
  virtual void set_type_id() {
    if (regex_pattern_hv_type_id == 0) regex_pattern_hv_type_id = ght("REGEX");
  }

  void initialise() {
    set_type_id(); 
    f_use_wildcard = false;
    f_deep_cmp = false;
  }
  
 public:
  using matchable_pattern::match;
  
 regex(): matchable_pattern(), vector<regex_element> () { initialise(); }
  virtual ~regex() {}

  virtual pattern* clone() const {
    regex* rp = new regex;
    for (const_iterator i=begin(); i!=end(); ++i) rp->push_back(*i);
    return rp;
  } 
  
  string to_str(const char* delim="  ", const char* lrb ="(", const char* option_sep="|", const char* rrb=")", const char* qmark="?") const;
  string to_xml() const;
  
  virtual string to_string(const string& delim="  ") const { return to_str( delim.c_str() ) ; }
  virtual string gen_pcre() const { return to_str("\\s+", "(?:", "|", ")", "?") ; }
  
  void push_back(const regex_element& re) ;
  
  void push_back(const vector<hash_value>& vhv) ;
  
  void push_back(hash_value h) ;
  
  void clear(void) { vector<regex_element>::clear(); }
  
  bool operator == (const regex& r1) const ;
  
  int deepsilon(void) ;
  
  bool is_flat() const ;
  
  void sort();
  
  virtual bool simplify(hash_value hv_wildcard = 0) ;
  
  static hash_value type_id() {return regex_pattern_hv_type_id;}
  virtual hash_value get_type_id() const { return regex_pattern_hv_type_id; }
  
  virtual bool is_numeric() const final { return false; }
  
  virtual bool match(const token_string&  ts) const ;
  
  virtual matched_pos find(const token_string&  ts, const matching_range& r) const ;
  
  //  virtual vector<size_t> find_all(const token_string&  ts) const ;
  
  virtual double atfidf(const sample_list& sl) const ;
};

char* parse_regex_xml(regex& out_re, hash_table& ht, const char* string);


#endif //  __regex_h 1
