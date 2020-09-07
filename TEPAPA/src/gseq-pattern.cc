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

#include <algorithm>

#include "gseq-pattern.h"
#include "tepapa.h"

hash_value gapped_sequence_pattern::gapped_sequence_pattern_hv_type_id = 0;

bool gapped_sequence_pattern::match(const token_string&  ts) const {
	return find(ts).first != ts.end();
	}
	
	
matched_pos gapped_sequence_pattern::find(const token_string&  ts, const matching_range& r) const {
	
  token_string::const_iterator s  = ts.begin() + r.from;
  token_string::const_iterator s0 = ts.end() ;
  token_string::const_iterator e  = ts.begin() + r.to;
  
	
  vector<ngram_pattern>::const_iterator npi = begin();
	
  while ( npi != end() ) {
    if (s >= e) return matched_pos( ts.end(), ts.end() ); // beyond matching range

    if ( s0 == ts.end() ) s0 = s;

    matched_pos m = npi->find( ts, matching_range(ts, s, e) );

    s = m.first;
    
    if ( s == ts.end() ) return matched_pos( ts.end(), ts.end() ); // not found? then return false;
      
    s += npi->size();
    
    npi++;
  }
  
  return matched_pos( s0, s+1 );
}


vector<size_t> gapped_sequence_pattern::find_all(const token_string&  ts, const matching_range& r) const {

  vector<size_t> retval ;
	
  matched_pos m ;
  
  while ( (m = find(ts, r)).first != ts.end() ) {
    retval.push_back( m.first - ts.begin() );
  }
  
  return retval ;
}
	



string gapped_sequence_pattern::to_string(const string& delim) const {
	string substr_str;
	
	for(unsigned int i=0; i<size(); ++i) { 
		if ( substr_str.length() > 0 ) {
			substr_str += delim;
			substr_str += ".*";
			substr_str += delim;
			}
		substr_str += (begin()+i)->to_string(delim)  ;
		}
	return substr_str ;
	}
	


bool gapped_sequence_pattern::has_subpat(const matchable_pattern& ssmp) const {
	const matchable_pattern* mp = &ssmp;
	const gapped_sequence_pattern* sspp = dynamic_cast<const gapped_sequence_pattern*>(mp);
	if (!sspp) return false;
	
	const gapped_sequence_pattern& ssp = *sspp ;
	
	if (!ssp.size()) return true;
	for(const_iterator i=begin(); i!=end(); ++i) {
		if ( *i == ssp.front() ) {
			for (unsigned int j=0; j<=ssp.size(); ++j) {
				if ( j == ssp.size() ) return true;
				if ( (i+j) == end() ) break;
				if ( *(i+j) != ssp[j] ) break;
				}
			}
		}
	return false;
	}


double gapped_sequence_pattern::atfidf(const sample_list& sl) const {
	double x = 0;
	for(const_iterator i=begin(); i!=end(); ++i) {
		x += i->atfidf( sl );
		}
	return x;
	}

string gapped_sequence_pattern::gen_pcre() const { 
	string s;
	for(const_iterator i=begin(); i!=end(); ++i) {
		if (s.length()) s += ".*";
		s += i->gen_pcre();
		}
	return s;
	}
