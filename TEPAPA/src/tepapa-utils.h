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

#ifndef __tepapa_utils
#define __tepapa_utils 1

#include "tepapa-results.h"

bool sort_by_pval(const TEPAPA_Result& a, const TEPAPA_Result& b );
bool sort_by_est(const TEPAPA_Result& a, const TEPAPA_Result& b ) ;
bool sort_by_ngram_length_desc(const TEPAPA_Result& a, const TEPAPA_Result& b) ;

#include "bprof.h"

binary_profile  match_type(const vector< TEPAPA_Result >& a, hash_value hv_type_id) ;

template <typename T> void append(vector<T>&  a, const vector<T>&  b) { a.insert (a.end(), b.begin(), b.end()); }

#include "utils.h"

string find_executable(const string& name, const string& directory, const string& prefix, const string& suffix);
string find_executable(const vector<string>& names, const vector<string>& directories, const vector<string>& prefixes, const vector<string>& suffixes);

#endif // __tepapa_utils
