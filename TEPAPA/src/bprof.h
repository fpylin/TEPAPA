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

#ifndef __bprof_h
#define __bprof_h 1

using namespace std;

#include <vector>
#include <string>
#include <assert.h>
#include "iptr.h"

struct binary_profile: public vector<bool>, public __iptr_member {
	binary_profile(): vector<bool>(), __iptr_member() {}
	binary_profile(int n): vector<bool> (n), __iptr_member() {}
	binary_profile(const char* hexdigest);
	binary_profile(const vector<double>& v) ;
	binary_profile(const vector<bool>& v) ;

	
	string digest() const;
	string bin_digest() const;
	string to_string() const { return digest(); }
	
	unsigned int npos() const ;
	
	void operator |= (const binary_profile& b1) {
		assert (size() == b1.size());
		for(register unsigned int i=0; i<size(); ++i) {
			if ( b1[i] )  *(begin()+i) = true;
			}
		}
	void operator &= (const binary_profile& b1) {
		assert (size() == b1.size());
		for(register unsigned int i=0; i<size(); ++i) {
			if ( ! b1[i] )  *(begin()+i) = false;
			}
		}

	void operator = (const bool tf) {
		for(register unsigned int i=0; i<size(); ++i) *(begin()+i) = tf;
		}
	
	void negate(void) {
		for(register unsigned int i=0; i<size(); ++i) {
			*(begin()+i) = ! *(begin()+i) ;
			}
		}
	
	binary_profile operator ! () const {
		binary_profile bp = *this;
		bp.negate();
		return bp;
		}
	
	void reset(void) { for(register unsigned int i=0; i<size(); ++i) *(begin()+i) = false; }
	
	bool contains(const binary_profile& b1) const {
		assert (size() == b1.size());
		for(register unsigned int i=0; i<size(); ++i) {
			if ( ! *(begin()+i) && b1[i] ) return false;
			}
		return true;
		}
	
	operator vector<double> () const;
	
	static double fisher_exact_pval(const binary_profile& bp1, const binary_profile& bp2);
	static double eucliean_distance(const binary_profile& bp1, const binary_profile& bp2);
	static double normalised_distance(const binary_profile& bp1, const binary_profile& bp2);
	};


bool is_mutex(const binary_profile& b1, const binary_profile& b2);


template <class T> vector<T> subset(const vector<T>& vvh, const binary_profile& bp) {
	vector<T> retval;
	assert(vvh.size() == bp.size());
	for(unsigned int i=0; i<bp.size(); ++i) {
		if (bp[i]) retval.push_back( vvh[i]);
		}
	return retval;
	}

#endif // __bprof_h
