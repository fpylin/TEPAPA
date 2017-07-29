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
#ifndef __fgroup_h
#define __fgroup_h 1

#include "tepapa-results.h"
#include "samples.h"


TEPAPA_Result select_best_pattern_by_atfidf(vector<double>& v_atfidf, const TEPAPA_Results& v, const sample_list& sl) ;

bool split_mergeable_subsets(TEPAPA_Results& out_v_mergeable, TEPAPA_Results& out_v_not_mergeable, const TEPAPA_Results& in) ;

template <typename T> int reduce_by_collapse_subpatterns(TEPAPA_Results& rr_out, map<T, TEPAPA_Results>&  results_by_signature , const sample_list& sl);

template <typename T> int reduce_by_collapse_subpatterns(TEPAPA_Results& rr_out, TEPAPA_Results& rr, const sample_list& sl) ;

template <typename T> int reduce_by_collapse_subpatterns(TEPAPA_Results& rr_out, TEPAPA_Results& rr) ;


#endif // __fgroup_h
