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

#include "tepapa-tester.h"
#include "bprof.h"
#include "samples.h"
#include "tepapa-msg.h"
#include "tepapa-numeric.h"
#include "parallel.h"
#include <typeinfo>

TEPAPA_Tester::TEPAPA_Tester()
	: __iptr_member() {
	sl_ptr = 0; 
	f_use_wildcard = false;
	f_use_deep_cmp = false;
	}

void TEPAPA_Tester::set_sample_list(sample_list& p_sl_ref) { 
	
	sl_ptr = & p_sl_ref;
	
	sample_list& sl = *sl_ptr;
        
	v_score.clear();
			
	for(unsigned int i=0; i<sl.size(); ++i)  v_score.push_back(sl[i].score);
	}


void TEPAPA_Tester::run1(unsigned int pstart, unsigned int pend) {
	
	const sample_list& sl = *( (const sample_list*)sl_ptr );
	
	for (unsigned int i=pstart; i<pend; ++i) {
		if (i >= patterns_to_test.size()) break;
		iptr<pattern> patt_ptr = patterns_to_test[i];
// 		pattern* patt_ptr = patterns_to_test[i];
		
		if ( ! match_type(patt_ptr) ) continue;
		
		TEPAPA_Tester_profile_set prof;
		
		if (! test( prof, sl, (pattern*)(patt_ptr) ) ) continue;
		
		if ( prof.vs.size() ) { // numeric 
			evaluator -> eval_numeric( prof.bp, v_score, prof.vs, patterns_to_test[i] );
			}
		else { // symbolic
		  evaluator -> eval_symbolic( prof.bp, outvar_vectors_t(v_score), patterns_to_test[i] );
			}
		}
	}

// void regex_inducer1(regex_list& retval, const vector<sequences>& substr_groups, ) {

/*
int TEPAPA_Tester::run() {
	
	int n=0;

#if TEPAPA_MULTITHREAD
	thread_manager tm;
#endif // TEPAPA_MULTITHREAD

	msgf(VL_INFO, "%u patterns to test\n", patterns_to_test.size());
	
	const int unit_size = 100;
	
	for (unsigned int i=0; i<patterns_to_test.size(); i += unit_size) {
		msgf(VL_DEBUG, "Testing pattern %u to %u \n", i,  i+unit_size - 1);
		
#if TEPAPA_MULTITHREAD
		tm.queue();
		tm.push_back( 
			std::async( std::launch::async, &TEPAPA_Tester::run1, this, i, i+unit_size) 
			);
#else	
		run1(i, i+unit_size);
#endif // TEPAPA_MULTITHREAD
		}
#if TEPAPA_MULTITHREAD
	tm.join_all();
#endif // TEPAPA_MULTITHREAD

	return n;
	}*/



int TEPAPA_Tester::run() {
	
	const sample_list& sl = *( (const sample_list*)sl_ptr );

	evaluator= new TEPAPA_Evaluator( sl );

	batch_process<iptr<pattern>>(patterns_to_test, [&](iptr<pattern>& p) -> void{
		if ( ! match_type(p) ) return;
		
		TEPAPA_Tester_profile_set prof;
		
		if (! test( prof, sl, (pattern*)(p) ) ) return;
		
		if ( prof.vs.size() ) { // numeric 
			evaluator -> eval_numeric( prof.bp, v_score, prof.vs, p );
			}
		else { // symbolic
  		  evaluator -> eval_symbolic( prof.bp, outvar_vectors_t(v_score), p );
			}
		},
		10
		);


	return patterns_to_test.size();
	}


	
////////////////////////////////
#include "linereg.h"

void TEPAPA_Program_Tester::calc_concordance(const TEPAPA_Results& results) const{
	msgf(VL_INFO, "Calculating concordance\n" ); 
	TEPAPA_global_data_struct& gvr = *gvr_ptr;
	
	TEPAPA_Results& rr_import = gvr.get_dataset(ds_import_patterns).rr;
	
	typedef map<TEPAPA_method_pattern_pair, vector<TEPAPA_Result>::const_iterator>   mrrm;
	mrrm  res_index = results.get_index();
	mrrm  rri_index = rr_import.get_index();
	mrrm::const_iterator  mrrmi, mrrmj;
	
	map<hash_value, points>  est_vec_by_method;
	
	for(mrrmi=res_index.begin(); mrrmi!= res_index.end(); ++mrrmi) {
		mrrmj = rri_index.find(mrrmi->first) ;
// 		printf("!>\t%s\t%s\n", ght[ hv_method ], ght[ hv_patt ]);		
		if ( mrrmj == rri_index.end() ) continue;
		hash_value hv_method = mrrmi->first.first;
		hash_value hv_patt = mrrmi->first.second;
		
		const TEPAPA_Result& res_i = *( mrrmi->second );
		const TEPAPA_Result& res_j = *( mrrmj->second );
		printf("TT-PAIR\t%s\t%s\t%f\t%f\t%f\t%f\n", ght[ hv_method ], ght[ hv_patt ], res_i.est, res_i.pval, res_j.est, res_j.pval );
		
		est_vec_by_method[hv_method].push_back( point(res_j.est, res_i.est) ) ;
		}
	
	for (map<hash_value, points>::const_iterator it=est_vec_by_method.begin(); it!=est_vec_by_method.end(); ++it) {
		hash_value hv_method = it->first;
		const points& est_vec = it->second;
		if ( est_vec.size() > 4 ) {
			reg_line rl = linear_regression(est_vec);
			printf("TT-CONCORDANCE\t%s\tBETA\t%g\tINTERCEPT\t%g\tR2\t%g\n", ght[ hv_method ], rl.m(), rl.c(), rl.r2() );
			}
		else {
			printf("TT-CONCORDANCE\t%s\tBETA\tNA\tINTERCEPT\tNA\tR2\tNA\n", ght[ hv_method ]);
			}
		}
	
	}

#include "meta-pattern.h"
#include "predicate-pattern.h"

int TEPAPA_Program_Tester::run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) {
	vector< iptr<TEPAPA_Tester> >  testers;
	
	testers.push_back( new TEPAPA_Tester_Matchable<ngram_pattern> );
	testers.push_back( new TEPAPA_Tester_Matchable<regex> );
	testers.push_back( new TEPAPA_Tester_Matchable<meta_pattern> );
	testers.push_back( new TEPAPA_Tester_Evaluable<extractable_ngram_pattern> ); // numeric 
	testers.push_back( new TEPAPA_Tester_Evaluable<predicate_pattern> ); // numeric 
	
	
	TEPAPA_Results results ;
	
	if ( ds_import_patterns.length() ) {
		set_test_patterns_from_dataset(ds_import_patterns);
		}
	else {
		for(unsigned int i=0; i<ds_input.rr.size(); ++i) {
			iptr<pattern>  ipptr = ds_input.rr[i].patt;
			ipptr -> simplify( (f_use_wildcard ? hv_wildcard : 0) );
			patterns_to_test.push_back( ipptr );
			}
		}

	for(vector< iptr<TEPAPA_Tester> >::iterator ti=testers.begin(); ti!=testers.end(); ++ti) {
		iptr<TEPAPA_Tester>&  t = *ti;
		
		t-> set_sample_list( ds_input.sl ) ;
		
		t-> set_use_wildcard( f_use_wildcard );
		t-> set_use_deep_cmp( f_use_deep_cmp );
		
		t-> set_test_patterns( patterns_to_test );
		
		t-> run();
		
		results.append( t-> get_results() );
		}

	if (f_calc_concordance) calc_concordance(results);
		
	rr_output = results;
	
	return TEPAPA_RETVAL_SUCCESS;
	}

	
void TEPAPA_Program_Tester::set_test_patterns_from_dataset(const string& ds_name) {
	TEPAPA_global_data_struct& gvr = *gvr_ptr;
	
	patterns_to_test.clear();
	
	string ds_name_copy_feature = try_expand_variable(ds_name); 
	
	msgf(VL_INFO, "Using feature from dataset [%s]\n", ds_name_copy_feature.c_str()); 

	TEPAPA_Results& rr = gvr.get_dataset(ds_name_copy_feature).rr;

	for(unsigned int i=0; i<rr.size(); ++i) {
		iptr<pattern>  ipptr = rr[i].patt;
		ipptr -> simplify( (f_use_wildcard ? hv_wildcard : 0) );
		patterns_to_test.push_back( ipptr );
		}
	}


TEPAPA_Program_Tester::TEPAPA_Program_Tester()
	: TEPAPA_Program("@Test") {
	f_use_wildcard = false;
	f_use_deep_cmp = false;
	f_calc_concordance = false;
	
	patterns_to_test.clear();
	
	options_binary.push_back(
		TEPAPA_option_binary("-w", "--use-wildcard", "", 'b', &f_use_wildcard)
		);
	
	options_binary.push_back(
		TEPAPA_option_binary("-d", "--use-deep-comparison", "", 'b', &f_use_deep_cmp) 
		);

	options_binary.push_back(
		TEPAPA_option_binary("-c", "--calc-concordance", "", 'b', &f_calc_concordance ) 
		);

	options_optarg.push_back(
		TEPAPA_option_optarg("-u", "--use-features-from-dataset", "", 's', &ds_import_patterns) 
		);
	}
