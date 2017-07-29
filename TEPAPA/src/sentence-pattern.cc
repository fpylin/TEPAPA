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
 * (at your option) any later veersion.
 *
 * TEPAPA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with TEPAPA.  If not, see <http://www.gnu.org/licenses/>
 */

#include "sentence-pattern.h"
#include "strmetric.h"

hash_value sentence_pattern::sentence_pattern_hv_type_id = 0;

vector<sentence_pattern>  sentence_pattern::convert_from(const token_string& ts) {
	vector<sentence_pattern> vrs;

	sentence_pattern  spat;
	
	for(unsigned int j=0; j<ts.size(); ++j) {
		hash_value hv = ts[j].primary();
		if ( ( hv_separators.find(hv) != hv_separators.end() ) ) {
			vrs.push_back(spat);
			spat.clear();
			continue;
			}
		spat.push_back( hv );
		}

	if ( spat.size() ) vrs.push_back(spat);
	
	return vrs;
	}

double sentence_pattern::calc_dissimilarity_metric(const sentence_pattern& rs1, const sentence_pattern& rs2, int type_dissimilarity_metric){
	int n1 = rs1.size();
	int n2 = rs2.size();
	int N = n1 + n2;
	if (N == 0) ++N;
// 	int d = levenshtein(rs1, rs2);
	int d = damerau_levenshtein(rs1, rs2);
	double f = double(d) / double(N);
// 	printf("%d\t%d\t%g\n", d, N, f);
	return f;
	}
