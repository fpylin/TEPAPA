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

#include "tepapa-reduce-infsubset.h"
#include "tepapa-msg.h"
#include "tepapa-utils.h"
#include <algorithm>


int merge_by_subset(TEPAPA_Results& rr) {
	TEPAPA_Results rr_processed;

	vector< TEPAPA_Result >  v ;
	
	for(unsigned int i=0; i<rr.size(); ++i) v.push_back( rr[i] );
	
	int n_before_merge = v.size();
	
	sort(v.begin(), v.end(), sort_by_pval);
	
	vector<bool> valid( v.size() ) ; 
	for(unsigned int i=0; i<v.size(); ++i) valid[i] = true;
	
	for(unsigned int i=0;i<v.size();++i) {
		if ( ! valid[i] ) continue;
		pattern* iptr =  v[i].patt;
		
		if ( iptr->get_type_id() != ngram_pattern::type_id() ) continue;
		
		ngram_pattern* np_iptr = dynamic_cast<ngram_pattern*>(iptr);
		
// 		double pval_i = v[i]->pval;
		
		for(unsigned int j=i+1;j<v.size();++j) {
			if ( ! valid[j] ) continue;
			pattern* jptr =  v[j].patt;
			
			if ( jptr ->get_type_id() != ngram_pattern::type_id() ) continue;
			
			ngram_pattern* np_jptr = dynamic_cast<ngram_pattern*>(jptr);
// 			fprintf(stderr, "%x\t%x\n", iptr, jptr);
// 			double pval_j = v[j]->pval;
			
			if ( ( np_iptr->has_subpat(*np_jptr) ) ) {
				msgf(VL_DEBUG, "MERGE\tSUBSTR_SUBSET\t%d\t%g\t%d\t%g\t%s\t<-\t%s\n", i, v[i].pval, j, v[j].pval, np_iptr->to_string().c_str(), np_jptr->to_string().c_str());
				valid[j] = false;
				continue;
				}
			
			if ( ( v[i].bprof.contains( v[j].bprof ) )  ) {
				msgf(VL_DEBUG, "MERGE\tBPROF_SUBSET\t%d\t%g\t%d\t%g\t%s\t<-\t%s\n", i, v[i].pval, j, v[j].pval, np_iptr->to_string().c_str(), np_jptr->to_string().c_str());
				valid[j] = false;
				continue;
				}
				
			}
		}
	
	for(unsigned int j=0; j<v.size(); ++j) {
		if ( ! valid[j] ) continue;
		string s = v[j].patt->to_string();
// 		pattern* vptr =  v[j]->patt;
// 		fprintf(stderr, "  |\t%d\t%s\n", dynamic_cast<ngram_pattern*>(vptr)->size(), s.c_str());
		rr_processed.push_back(v[j]);
		}

	int n_after_merge = rr_processed.size();
	msgf(VL_DEBUG, "Before merge: %d\n", n_before_merge);
	msgf(VL_DEBUG, "After merge: %d\n", n_after_merge );
	
	rr = rr_processed;
	
	return 1;
	}


TEPAPA_Program_Reduce_Inferior_Subsets::TEPAPA_Program_Reduce_Inferior_Subsets()
	: TEPAPA_Program("@ReduceInferiorSubsets") {
	}

	
int TEPAPA_Program_Reduce_Inferior_Subsets::run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) {
	
	TEPAPA_Results  rr = ds_input.rr;

	merge_by_subset(rr) ;
	
	rr_output = rr;

	return TEPAPA_RETVAL_SUCCESS;
	}
