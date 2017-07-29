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

bool matchable_pattern::match(const token_string&  ts) const {
	return ( find(ts) != ts.end() ); 
	}


token_string::const_iterator matchable_pattern::find(const token_string&  ts) const { 
	return find(ts, ts.begin()); 
	}


vector<int> matchable_pattern::find_all(const token_string&  ts) const {
	vector<int>  retval;
	
	token_string::const_iterator s0 = ts.begin();
	
	for( token_string::const_iterator z=s0; z != ts.end(); ++z ) {
		token_string::const_iterator y = find(ts, z) ;
		if ( y == ts.end() ) break;
		int i = y - s0;
		retval.push_back( i );
		z = y;
		}
	
	return retval;
	}


binary_profile matchable_pattern::match(const sample_list& sl) const {
	binary_profile  retval;
	
	for(sample_list::const_iterator i=sl.begin(); i!=sl.end(); ++i) {
		retval.push_back( match( i->data ) ); 
		}
		
	return retval;
	}


binary_profile matchable_pattern::match(const sample_list& sl, const binary_profile& search_mask) const {
	
	assert( sl.size() == search_mask.size() );
	
	binary_profile  retval( sl.size() );
	
	int ii=0;
	
	for(sample_list::const_iterator i=sl.begin(); i!=sl.end(); ++i, ++ii) {
		if ( search_mask[ii] ) {
			retval[ii] = match( i->data ); // search with index
			}
		else {
			retval[ii] = false;
			}
		}
	return retval;
	}

