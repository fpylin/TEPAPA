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
#include "matchable-pattern.h"


matching_range::matching_range(const token_string& ts) {
  from = 0;
  to = ts.end() - ts.begin();
}


matching_range::matching_range(const token_string& ts, const token_string::iterator ifrom) {
  from = ifrom - ts.begin();
  to = ts.end() - ifrom;
}

matching_range::matching_range(const token_string& ts, const token_string::const_iterator ifrom) {
  from = ifrom - ts.begin();
  to = ts.end() - ifrom;
}

matching_range::matching_range(const token_string& ts, const token_string::const_iterator ifrom, const token_string::const_iterator ito) {
  from = ifrom - ts.begin();
  to = ito - ts.begin();
}


bool matchable_pattern::match(const token_string&  ts, const matching_range& r) const {
  return ( find(ts, r).first != ts.end() ); 
}


bool matchable_pattern::match(const token_string&  ts) const {  
  return match(ts, matching_range(ts));
}


matched_pos matchable_pattern::find(const token_string&  ts) const { 
  return find(ts, matching_range(ts)); 
}


vector<size_t> matchable_pattern::find_all(const token_string&  ts, const matching_range& r) const {
  vector<size_t>  retval;
	
  token_string::const_iterator s0 = ts.begin() + r.from;
  token_string::const_iterator s1 = ts.begin() + r.to;
  
  for( token_string::const_iterator z=s0; z < s1; ++z ) {
    matched_pos y = find(ts, matching_range(r.from, r.to)) ;
    if ( y.first == ts.end() ) break;
    size_t i = std::distance(s0, y.first);
    retval.push_back( i );
    z = y.first;
  }
	
  return retval;
}
  

vector<size_t> matchable_pattern::find_all(const token_string&  ts) const {
  return find_all(ts, matching_range(ts));
}


binary_profile matchable_pattern::match(const sample_list& sl) const {
  binary_profile  retval;
	
  for(sample_list::const_iterator i=sl.begin(); i!=sl.end(); ++i) {
    retval.push_back( match( i->data ) ); 
  }
  
  return retval;
}


binary_profile matchable_pattern::match(const sample_list& sl, const matching_range_list& rl) const {
  assert( sl.size() == rl.size() );

  binary_profile  retval( sl.size() );
  
  matching_range_list::const_iterator rli = rl.begin();
  
  for(sample_list::const_iterator i=sl.begin(); i!=sl.end(); ++i, ++rli) {
    retval.push_back( match( i->data, *rli) ); 
  }
  
  return retval;
}



binary_profile matchable_pattern::match(const sample_list& sl, const binary_profile& search_mask) const {	
  assert( sl.size() == search_mask.size() );
  
  binary_profile  retval( sl.size() );
  
  unsigned int ii=0;
  
  for(sample_list::const_iterator i=sl.begin(); i!=sl.end(); ++i, ++ii) {
// 	fprintf(stderr, "%d T  E !!\n", ii);
    if ( ( search_mask[ii] ) ) {
      retval[ii] = match( i->data ); 
    }
    else {
      retval[ii] = false;
    }
  }
  return retval;
}


binary_profile matchable_pattern::match(const sample_list& sl, const binary_profile& search_mask, const matching_range_list& rl) const {
  assert( sl.size() == search_mask.size() );
  assert( sl.size() == rl.size() );
  
  binary_profile  retval( sl.size() );
  
  int ii=0;
  matching_range_list::const_iterator rli = rl.begin();
  
  for(sample_list::const_iterator i=sl.begin(); i!=sl.end(); ++i, ++ii, ++rli) {
    if ( search_mask[ii] ) {
      retval[ii] = match( i->data, *rli ); 
    }
    else {
      retval[ii] = false;
    }
  }
  return retval;
}
