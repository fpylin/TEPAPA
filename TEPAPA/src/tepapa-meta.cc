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
#include "tepapa-meta.h"
#include "tepapa-msg.h"
#include "ngram-pattern.h"
#include "samples.h"
#include "tepapa-results.h"

#include "fgroup.h"

/* FIXME: Current BUGGY! DO NOT USE
 */

TEPAPA_Program_Meta_Pattern_Learner::TEPAPA_Program_Meta_Pattern_Learner() :
	TEPAPA_Program("@DiscoverMetaPatterns") {
		
	depth = 1;
	f_gen_by_cooc_sig = false;
	
	options_optarg.push_back( 
		TEPAPA_option_optarg ("-d", "--depth", "", 'i', &depth)
		);
	
	options_binary.push_back( 
		TEPAPA_option_binary("-g", "--generate-by-cooccurrence-signature", "", 'b', &f_gen_by_cooc_sig)
		);
	
	}


MPL_retval_struct TEPAPA_Program_Meta_Pattern_Learner::MPL_AND(const TEPAPA_Result& r1, const TEPAPA_Result& r2) {
	MPL_retval_struct  retval;
	retval.bprof_out  = r1.bprof;
	retval.bprof_out &= r2.bprof;
	
	if ( retval.bprof_out == r1.bprof ) return retval;
	if ( retval.bprof_out == r2.bprof ) return retval;
		
	if ( retval.bprof_out.npos() <= 1 || retval.bprof_out.npos() >= retval.bprof_out.size() - 1 ) return retval;
	
	retval.mp = new meta_pattern( ght("AND") );
	retval.mp->push_back( r1.patt );
	retval.mp->push_back( r2.patt );
	
	return retval;
	}

MPL_retval_struct TEPAPA_Program_Meta_Pattern_Learner::MPL_FOLLOW(const sample_list& sl, const TEPAPA_Result& r1, const TEPAPA_Result& r2) {
	MPL_retval_struct  retval;
	retval.bprof_out  = r1.bprof;
	retval.bprof_out &= r2.bprof;
	
	if ( retval.bprof_out == r1.bprof ) return retval;
	if ( retval.bprof_out == r2.bprof ) return retval;
		
	if ( retval.bprof_out.npos() <= 1 || retval.bprof_out.npos() >= retval.bprof_out.size() - 1 ) return retval;
	
	for(unsigned int i=0; i<sl.size(); ++i) {
		if ( ! retval.bprof_out[i] )  continue;
		pattern* pp1 = r1.patt ;
		ngram_pattern* ngp1 = dynamic_cast<ngram_pattern*>(pp1);
		if (! ngp1) {
			retval.bprof_out[i] = false;
			continue;
			}
		
		pattern* pp2 = r2.patt ;
		ngram_pattern* ngp2 = dynamic_cast<ngram_pattern*>(pp2);
		if (! ngp2) {
			retval.bprof_out[i] = false;
			continue;
			}
		
		vector<int> vi1 = ngp1->find_all( sl[i].data );
		vector<int> vi2 = ngp2->find_all( sl[i].data );
		
		bool f_found = false;
		for(unsigned int j1=0; j1<vi1.size(); j1++) {
			for(unsigned int j2=0; j2<vi2.size(); j2++) {
				if (vi1[j1] < vi2[j2]) {
					f_found = true;
					break;
					}
				if (f_found) break;
				}
			if (f_found) break;
			}
		if (! f_found) retval.bprof_out[i] = false;
		}

	retval.mp = new meta_pattern( ght("FOLLOW") );
	retval.mp->push_back( r1.patt );
	retval.mp->push_back( r2.patt );
	
	return retval;
	}

MPL_retval_struct TEPAPA_Program_Meta_Pattern_Learner::MPL_OR(const TEPAPA_Result& r1, const TEPAPA_Result& r2) {
	MPL_retval_struct  retval;
	retval.bprof_out  = r1.bprof;
	retval.bprof_out |= r2.bprof;
	
	if ( retval.bprof_out == r1.bprof ) return retval;
	if ( retval.bprof_out == r2.bprof ) return retval;
		
	if ( retval.bprof_out.npos() <= 1 || retval.bprof_out.npos() >= retval.bprof_out.size() - 1 ) return retval;
	
	retval.mp = new meta_pattern( ght("OR") );
	retval.mp->push_back( r1.patt );
	retval.mp->push_back( r2.patt );
	
	return retval;
	}

MPL_retval_struct TEPAPA_Program_Meta_Pattern_Learner::MPL_NOT(const TEPAPA_Result& r1) {
	MPL_retval_struct  retval;
	
	retval.bprof_out = ! r1.bprof;
	
	if ( retval.bprof_out == r1.bprof ) return retval;
		
	if ( retval.bprof_out.npos() <= 1 || retval.bprof_out.npos() >= retval.bprof_out.size() - 1 ) return retval;
	
	retval.mp = new meta_pattern( ght("NOT") );
	retval.mp->push_back( r1.patt );
	
	return retval;
	}


bool TEPAPA_Program_Meta_Pattern_Learner::handle_argv(const vector<string>& argv) {
	
	fmts = argv;
	
	return true;
	}


bool TEPAPA_Program_Meta_Pattern_Learner::do_evaluate(const MPL_retval_struct& pat_match) {
		
	if (!pat_match.mp) return false;
	
	iptr<pattern> pp = pat_match.mp;
	
	evaluator -> eval_symbolic( pat_match.bprof_out, v_score, pp );
	
	return true;
	}
	
void TEPAPA_Program_Meta_Pattern_Learner::set_v_score(const sample_list& sl) {
	
	v_score.clear();
	
	for(unsigned int i=0; i<sl.size(); ++i)  v_score.push_back(sl[i].score);
	
	vector<double>  v_score_levels = get_levels( v_score ); 
	
	f_is_binary = ( v_score_levels.size() == 2 );
	}
	


int TEPAPA_Program_Meta_Pattern_Learner::run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) {
	
	TEPAPA_Results  rr = ds_input.rr;
	TEPAPA_Results  rr_new ;

	if (f_gen_by_cooc_sig) {
		if ( gen_meta_pattern_by_cooccurrence_signature(rr_new, rr) ) {
			rr_output = rr_new ;
			}

		return TEPAPA_RETVAL_SUCCESS;
		}
	
	set_v_score(ds_input.sl);
	
	evaluator = new TEPAPA_Evaluator( ds_input.sl.is_outvar_binary() );
	
	for(int d=0; d<depth; ++d) {
		msgf(VL_DEBUG, "Processing depth %lu:\n", d);
		evaluator -> reset();
		
		int before = rr.size();
		reduce_by_collapse_subpatterns<binary_profile>(rr, rr, ds_input.sl) ;
		
		int after = rr.size();
		
		msgf(VL_DEBUG, "Reduced from %lu to %lu features.\n", before, after);
		
		for (unsigned int k=0; k<fmts.size(); ++k) {
			if ( fmts[k] == string("NOT") ) {
				for(unsigned int i=0; i<rr.size(); ++i) {
					do_evaluate( MPL_NOT( rr[i] ) ) ;
					}
				continue;
				}
					
			if ( fmts[k] == string("AND") ) {
				for(unsigned int i=0; i<rr.size(); ++i) {
					for(unsigned int j=i+1; j<rr.size(); ++j) {
						do_evaluate( MPL_AND( rr[i], rr[j] ) ) ;
						}
					}
				continue;
				}
			
			if ( fmts[k] == string("OR") ) {
				for(unsigned int i=0; i<rr.size(); ++i) {
					for(unsigned int j=i+1; j<rr.size(); ++j) {
						do_evaluate( MPL_OR( rr[i], rr[j] ) ) ;
						}
					}
				continue;
				}
				
			if ( fmts[k] == string("FOLLOW") ) {
				for(unsigned int i=0; i<rr.size(); ++i) {
					for(unsigned int j=0; j<rr.size(); ++j) {
						if (i == j) continue;
						do_evaluate( MPL_FOLLOW( ds_input.sl, rr[i], rr[j] ) ) ;
						}
					}
				continue;
				}
			}
			
		TEPAPA_Results rr1 = evaluator -> get_results();
		
		reduce_by_collapse_subpatterns<binary_profile>(rr1, rr1, ds_input.sl) ;
		
		rr.append( rr1 );
		rr_new.append( rr1 );
		}
	
	rr_output = rr_new ;

	return TEPAPA_RETVAL_SUCCESS;
	}

#include "tepapa-utils.h"


int TEPAPA_Program_Meta_Pattern_Learner::gen_meta_pattern_by_cooccurrence_signature(TEPAPA_Results& rr_out, const TEPAPA_Results& rr) {
	
	rr_out.clear();
	
	map<binary_profile, TEPAPA_Results>  results_by_bprof = rr.group_by_signature<binary_profile>();
	map<binary_profile, TEPAPA_Results> ::iterator  k;
	
	int group_id =1;
	
	for(k=results_by_bprof.begin(); k!=results_by_bprof.end(); ++k, ++group_id) {
	
		if (k->second.size() <= 1)  {
			rr_out.push_back( k->second.front() );
			continue;
			}
		
		TEPAPA_Results v, nv;
		
		split_mergeable_subsets(v, nv, k->second);
		
		v = v.collapse_subpatterns();

		if (v.size() <= 1)  continue;
		
		meta_pattern* mp = new meta_pattern( ght("OR") );
		
		for( unsigned int i=0; i<v.size(); ++i )  mp->push_back( v[i].patt );
	
		TEPAPA_Result  v_new;
		v_new.patt = mp;
		
		rr_out.push_back( v_new );
		
		append(rr_out, nv);
		}
	
	return 1;
	}
