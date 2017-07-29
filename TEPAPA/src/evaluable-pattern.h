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
#ifndef __evaluable_pattern_h
#define __evaluable_pattern_h 1

#include "pattern.h"
#include "token.h"
#include "samples.h"

template <class RETVAL_TYPE> class evaluable_pattern: virtual public pattern {
	public:
	evaluable_pattern(): pattern () { }
	
	virtual ~evaluable_pattern() {}
	
	virtual bool is_invalid(RETVAL_TYPE v) const =0;
	
	virtual RETVAL_TYPE eval(const token_string&  ts) const =0;
	
	virtual vector<RETVAL_TYPE> eval(const sample_list& sl, binary_profile&  out_bp_mask) const ;
	};


template <class RETVAL_TYPE> vector<RETVAL_TYPE> evaluable_pattern<RETVAL_TYPE>::eval(const sample_list& sl, binary_profile&  out_bp_mask) const {
	
	vector<RETVAL_TYPE> retval( sl.size() );

	out_bp_mask.resize( sl.size() );

	for(unsigned int i=0; i<sl.size(); ++i) {
		const token_string& ts = sl[i].data;
		RETVAL_TYPE v = eval(ts);
		retval[i] = v;
		out_bp_mask[i] = is_invalid(v) ? false : true; 
		}

	return retval;
	}

#endif // __evaluable_pattern_h
