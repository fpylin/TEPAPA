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

#include "tepapa-gvar.h"
#include "tepapa-regexi.h"
#include "regexi2.h"

#include "tepapa-msg.h"

bool induce_regex_from_ngram_patterns(TEPAPA_Results& out_regex_results, const vector< iptr<pattern> >&  vnp) {
	
	vector<ngram_pattern>  np;
	
	for (unsigned int i=0; i<vnp.size(); ++i) {
		iptr<pattern>  ipptr = vnp[i];
		if ( ipptr->get_type_id() != ngram_pattern::type_id() ) continue;
		pattern* pptr = ipptr ;
		ngram_pattern* npptr = dynamic_cast<ngram_pattern*>(pptr);
		np.push_back(*npptr);
		}
	
	regex_list rl = regex_inducer(np);
	
	for(unsigned int i=0; i<rl.size(); ++i) {
		
		bool f_flat = rl[i].is_flat();
		
		msgf(VL_DEBUG, "Regex %4d\t%s\t%s\n", i, f_flat ? "(Flat)" : "", rl[i].to_str().c_str() );
		
		if (f_flat) continue;
		
		TEPAPA_Result r ;
		
		r.patt = new regex(rl[i]);
		
		out_regex_results.push_back( r );
		}
	
	return true;
	}


TEPAPA_Program_RegexInducer::TEPAPA_Program_RegexInducer()
	: TEPAPA_Program("@InduceRegex") 
	{
	f_by_subgroups = false;
	f_recursive_reduction = false;
	
	options_binary.push_back(
		TEPAPA_option_binary("-g", "--by-subgroups", "", 'b', &f_by_subgroups)
		);

	options_binary.push_back(
		TEPAPA_option_binary("-r", "--recursive-reduction", "", 'b', &f_recursive_reduction)
		);
	}


void recursive_reduce(vector<regex>& out, const regex& r, unsigned int cur) {
	regex r0 = r;
	
	while( cur < r.size() ) {
		if ( r[cur].options.size() > 2 ) {
			for(unsigned int i=0; i < r[cur].options.size(); i++) {
				regex r1 = r;
				r1[cur].options.erase( r1[cur].options.begin() + cur );
				out.push_back(r1);
				recursive_reduce(out, r1, cur);
				}
			}
		++cur;
		}
	}


int TEPAPA_Program_RegexInducer::run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) {
	
	TEPAPA_Results& rr = ds_input.rr;

	TEPAPA_Results regex_results;
	
	if ( ! f_by_subgroups ) {
		vector< iptr<pattern> >  input;
		
		for (unsigned int i=0; i<rr.size(); ++i) input.push_back( rr[i].patt );
		
		induce_regex_from_ngram_patterns(regex_results, input);
		}
	else {
		map<int, vector< iptr<pattern> > >  groups;
		
		for (unsigned int i=0; i<rr.size(); ++i) {
			groups[ rr[i].group ].push_back( rr[i].patt );
			}
			
		map<int, vector< iptr<pattern> > >::iterator g;
		for (g=groups.begin(); g!=groups.end(); ++g) {
			vector< iptr<pattern> >  input;
		
			for (unsigned int i=0; i<g->second.size(); ++i) input.push_back( (g->second)[i] );
		
			induce_regex_from_ngram_patterns(regex_results, input);
			}
		}

	if (f_recursive_reduction) {
		// This is a bad idea, as it generates N! possibilities
		for(unsigned int i=0; i<regex_results.size(); ++i) {
			pattern* p = regex_results[i].patt;
			regex* rp = dynamic_cast<regex*>(p);
			if (!rp) continue;
			const regex& r = *rp;
			vector<regex>  res;
			recursive_reduce(res, r, 0); 
			}
		}

	rr_output = regex_results;
	
	return TEPAPA_RETVAL_SUCCESS;
	};



//////////////////////////////////////////////////////////////////////////////////////////////////////////////


TEPAPA_Program_Discoverer_Regex::TEPAPA_Program_Discoverer_Regex(): 
	TEPAPA_Program_Discoverer ("@DiscoverRegex") {
	
	options_binary.push_back(
		TEPAPA_option_binary(TEPAPA_ARGSTR_DISCOVERER_REGEX_BY_SUBGROUPS, "--by-subgroups", "", 'b')
		);

	options_binary.push_back(
		TEPAPA_option_binary(TEPAPA_ARGSTR_DISCOVERER_REGEX_RECURSIVE_REDUCTION, "--recursive-reduction", "", 'b')
		);
	
	options_binary.push_back(
		TEPAPA_option_binary(TEPAPA_ARGSTR_DISCOVERER_REGEX_GREEDY_HILLCLIMB, "--greedy-hillclimb", "", 'b')
		);
	
	
	}


bool TEPAPA_Discoverer_Regex::eval_regex(iptr<TEPAPA_Evaluator>& e, const sample_list& sl, regex& r, binary_profile& search_mask, double* ppval ) {
	
	binary_profile bp = r.match(sl, search_mask);
	
	hash_value h = r.get_uniq_id();
	
	unsigned int nprpi = bp.npos();
	
	hr[h].set(nprpi); // mark as evaluated and record stats
	
	if ( (nprpi > 1)  ) {
		iptr<pattern> pp = new regex(r);
		
		e -> eval_symbolic( bp, v_score, pp, ppval );
		
		++total_cnt_cls;
		}
	
	search_mask = bp;
	
	return true;
	}


bool TEPAPA_Discoverer_Regex::eval_regex(iptr<TEPAPA_Evaluator>& e, const sample_list& sl, regex& r, double* ppval ) {
	
	binary_profile bp = r.match(sl);
	
	hash_value h = r.get_uniq_id();
	
	unsigned int nprpi = bp.npos();
	
	hr[h].set(nprpi); // mark as evaluated and record stats
	
	if ( (nprpi > 1)  ) {
		iptr<pattern> pp = new regex(r);
		
		e -> eval_symbolic( bp, v_score, pp, ppval );
		
		++total_cnt_cls;
		}
	
	return true;
	}



void TEPAPA_Discoverer_Regex::recursive_reduce_exhaustive(iptr<TEPAPA_Evaluator>& e, const sample_list& sl, regex& r, unsigned int cur, binary_profile& search_mask, regex& best_regex, double& best_pval) {
	
	double base_pval = 1.0;
	
	eval_regex(e, sl, r, search_mask, &base_pval);
	
	if (base_pval < best_pval) {
		best_pval = base_pval ;
		best_regex = r;
		}
	
	while( cur < r.size() ) {
		unsigned int n_ro = r[cur].options.size() ;
		
		if ( n_ro > 2 ) {
			
			for(unsigned int i=0; i < n_ro ; i++) {
				regex r1 = r;
				
				r1[cur].options.erase( r1[cur].options.begin() + i );
				
				binary_profile search_mask_new = search_mask;
				
				double new_pval = 1.0;
				eval_regex(e, sl, r1, search_mask_new, &new_pval);
				
				if (new_pval < best_pval) {
					best_pval = new_pval ;
					best_regex = r1;
					}
				
				bool flag = (base_pval > new_pval) ;
// 				const char* flag_str = ( flag ? "" : "[X]" );
				
				
				if ( flag && (r1[cur].options.size() > 2) ) {
// 					printf( "%d\t%d\t%s (p=%g) -> %s (p=%g) %s\n", 
// 						cur, i, 
// 						r.to_str().c_str(), base_pval,
// 						r1.to_str().c_str(), new_pval,
// 						flag_str
// 						);
				
					recursive_reduce_exhaustive(e, sl, r1, cur, search_mask_new, best_regex, best_pval);
					}
				}
			}
		++cur;
		}
	}



regex TEPAPA_Discoverer_Regex::recursive_reduce_greedy(const sample_list& sl, regex& r0) {

	iptr<TEPAPA_Evaluator> e = new TEPAPA_Evaluator( sl.is_outvar_binary() ) ;
	
	binary_profile search_mask(sl.size());
	search_mask = true;
	
	regex r = r0;
	
	int n_change = 0;
	int iter = 0;
	do	{
		++iter;
		n_change = 0;
		
		regex  better_regex;
		double better_regex_pval = 100;
		
		double r_pval = 100.0;
		
		eval_regex(e, sl, r, search_mask, &r_pval);
// 		msgf(VL_DEBUG, "#%d: \t%s %s\n", iter, search_mask.digest().c_str(), r.to_str().c_str() );
		
		for(unsigned int cur=0; cur<r.size(); ++cur) {
			
			unsigned int n_ro = r[cur].options.size() ;
			
			if ( n_ro <= 2 ) continue;
			
			for(unsigned int i=0; i < n_ro ; i++) {
				regex r1 = r;
				
				r1[cur].options.erase( r1[cur].options.begin() + i );
				
				binary_profile search_mask_new = search_mask;
				
				double r1_pval = 100.0;
				eval_regex(e, sl, r1, search_mask_new, &r1_pval);
				
				if ( r1_pval < r_pval ) {
					if ( r1_pval < better_regex_pval ) {
						better_regex = r1;
						better_regex_pval = r1_pval ;
						++n_change;
						}
// 					msgf(VL_DEBUG, "#%d %d %d (p=%g -> %g) %g %g\n\t%s %s ->\n\t%s %s\n", iter, cur, i, r_pval, r1_pval, (r_pval - r1_pval), better_regex_pval, 
// 							search_mask.digest().c_str(), r.to_str().c_str(), 
// 							search_mask_new.digest().c_str(), r1.to_str().c_str() );
					}
				}
			}
// 		msgf(VL_DEBUG, "   n_change = %d\tbetter_regex_pval=%g\n", n_change, better_regex_pval );
		if (n_change) r = better_regex;
		}
	while(n_change);
	return r;
	}


#include "parallel.h"

bool TEPAPA_Discoverer_Regex::run1_meta(const sample_list& sl, const TEPAPA_Results& rr) {
	
	TEPAPA_Results regex_results;
	
	if ( ! f_by_subgroups ) {
		vector< iptr<pattern> >  input;
		
		for (unsigned int i=0; i<rr.size(); ++i) input.push_back( rr[i].patt );
		
		induce_regex_from_ngram_patterns(regex_results, input);
		}
	else {
		map<int, vector< iptr<pattern> > >  groups;
		
		for (unsigned int i=0; i<rr.size(); ++i) {
			groups[ rr[i].group ].push_back( rr[i].patt );
			}
			
		map<int, vector< iptr<pattern> > >::iterator g;
		for (g=groups.begin(); g!=groups.end(); ++g) {
			vector< iptr<pattern> >  input;
		
			for (unsigned int i=0; i<g->second.size(); ++i) input.push_back( (g->second)[i] );
		
			induce_regex_from_ngram_patterns(regex_results, input);
			}
		}

	if (f_recursive_reduction) { // This is a bad idea, as it generates N! possibilities
		for(unsigned int i=0; i<regex_results.size(); ++i) {
			pattern* p = regex_results[i].patt;
			regex* rp = dynamic_cast<regex*>(p);
			if (!rp) continue;
			const regex& r = *rp;
			vector<regex>  res;
			recursive_reduce(res, r, 0); 
			}
		}


	if(f_greedy_hillclimb) {
		
// 		for(unsigned int i=0; i<regex_results.size(); ++i)  {
// 			pattern* p = regex_results[i].patt;
// 			regex* rp = dynamic_cast<regex*>(p);
// 			if (!rp) return false;
// 			regex& r = *rp;
// 			
// 			regex reduced_regex = recursive_reduce_greedy(sl, r);
// 			
// 			eval_regex(evaluator, sl, reduced_regex);
// 			}
		
		batch_process_const<TEPAPA_Result>((vector<TEPAPA_Result>&)regex_results, [&](const TEPAPA_Result& rr) -> void{
				pattern* p = rr.patt;
				regex* rp = dynamic_cast<regex*>(p);
				if (!rp) return ;
				regex& r = *rp;
				
				regex reduced_regex = recursive_reduce_greedy(sl, r);
				
				eval_regex(evaluator, sl, reduced_regex);
				},
			100
			);
	
// 		for(unsigned int i=0; i<regex_results.size(); ++i)  {
// 			pattern* p = regex_results[i].patt;
// 			regex* rp = dynamic_cast<regex*>(p);
// 			if (!rp) return false;
// 			regex& r = *rp;
// 			
// 			regex reduced_regex = recursive_reduce_greedy(sl, r);
// 			
// 			eval_regex(evaluator, sl, reduced_regex);
// 			}
		}
	else 
		{
		for(unsigned int i=0; i<regex_results.size(); ++i)  {
			pattern* p = regex_results[i].patt;
			regex* rp = dynamic_cast<regex*>(p);
			if (!rp) return false;
			regex& r = *rp;
			
			eval_regex(evaluator, sl, r );
			}
		}


	msgf(VL_NOTICE, "Total = %lu results\n", total_cnt_cls);

	return (total_cnt_cls != 0 ? true : false) ;
	}
