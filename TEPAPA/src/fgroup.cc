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

#include "fgroup.h"
#include "tepapa-msg.h"
#include "matchable-pattern.h"
#include "ngram-pattern.h"
#include "regex.h"
#include "bprof.h"
#include "tepapa-utils.h"
#include "regex.h"
#include "meta-pattern.h"
#include "parallel.h"	

#include <algorithm>


template int reduce_by_collapse_subpatterns<binary_profile>(TEPAPA_Results& rr_out, map<binary_profile, TEPAPA_Results>& m, const sample_list& sl) ;
template int reduce_by_collapse_subpatterns<binary_profile>(TEPAPA_Results& rr_out, TEPAPA_Results& rr, const sample_list& sl) ;
template int reduce_by_collapse_subpatterns<binary_profile>(TEPAPA_Results& rr_out, TEPAPA_Results& rr) ;

template int reduce_by_collapse_subpatterns<pval_est_pair>(TEPAPA_Results& rr_out, map<pval_est_pair, TEPAPA_Results>& m, const sample_list& sl) ;
template int reduce_by_collapse_subpatterns<pval_est_pair>(TEPAPA_Results& rr_out, TEPAPA_Results& rr, const sample_list& sl) ;
template int reduce_by_collapse_subpatterns<pval_est_pair>(TEPAPA_Results& rr_out, TEPAPA_Results& rr) ;

template int reduce_by_collapse_subpatterns<int>(TEPAPA_Results& rr_out, map<int, TEPAPA_Results>& m, const sample_list& sl) ;
template int reduce_by_collapse_subpatterns<int>(TEPAPA_Results& rr_out, TEPAPA_Results& rr, const sample_list& sl) ;
template int reduce_by_collapse_subpatterns<int>(TEPAPA_Results& rr_out, TEPAPA_Results& rr) ;

template <typename T> string get_group_signature(const T& o) { return o.to_string(); }
template <> string get_group_signature<int>(const int& o) { return strfmt("GROUP%04d", o); }


TEPAPA_Result select_best_pattern_by_atfidf(vector<double>& v_atfidf, const TEPAPA_Results& v, const sample_list& sl) {
	unsigned int best_at = 0;
	double best_atfidf = 0;

	for(unsigned int j=0; j<v.size(); ++j) {
		pattern* vptr =  v[j].patt;
		matchable_pattern* ngp_vptr =  dynamic_cast<matchable_pattern*>( vptr );
			
		double a = ngp_vptr->atfidf(sl);
		v_atfidf.push_back(a);
		if( a > best_atfidf ) {
			best_at = j;
			best_atfidf = a;
			}
		}
		
	return v[best_at];
	}


bool split_mergeable_subsets(TEPAPA_Results& out_v_mergeable, TEPAPA_Results& out_v_not_mergeable, const TEPAPA_Results& in) {
	binary_profile  res_filter ;
	
	res_filter  = match_type(in, ngram_pattern::type_id() );
	res_filter |= match_type(in, regex::type_id() );
	res_filter |= match_type(in, meta_pattern::type_id() );
	
	out_v_mergeable     = in.subset(res_filter) ;
	out_v_not_mergeable = in.subset(! res_filter) ;
	
	return true;
	}


void collapse_subpatterns(const string& group_signature, int group_id, TEPAPA_Results& rr_out, const TEPAPA_Results& v0, const sample_list& sl) {
	
	TEPAPA_Results v, nv;
	TEPAPA_Result br ;
		
	split_mergeable_subsets(v, nv, v0);
	
	v = v.collapse_subpatterns();
	
	vector<double> v_atfidf;
	
	if (v.size() == 1) {
		br = v.front() ;
		}
	else {
		br = select_best_pattern_by_atfidf(v_atfidf, v, sl); 
		}

	
	msgf(VL_DEBUG, "GROUP %06d: %s\t%u -> %u -> %u\n", group_id, group_signature.c_str(), v0.size(), v.size(), 1);
	
	if ( v_atfidf.size() )  {
		for(unsigned int j=0; j<v.size(); ++j) {
			bool f_best = ( v[j].patt->get_uniq_id() == br.patt->get_uniq_id() );
			msgf(VL_DEBUG, "%s\t%g\t%s\n", (f_best ? "***": ""), v_atfidf[j], ght[ v[j].patt->get_uniq_id() ] );
			}
		}

	rr_out.push_back_safe( br );
	rr_out.append(nv);
	}
	
	
template <typename T> int reduce_by_collapse_subpatterns(TEPAPA_Results& rr_out, map<T, TEPAPA_Results>&  results_by_signature, const sample_list& sl) {
	
	typename map<T, TEPAPA_Results> ::iterator  k;
	
	int group_id =1;

#if TEPAPA_MULTITHREAD 
	thread_manager  tm;
#endif // TEPAPA_MULTITHREAD
	
	for(k=results_by_signature.begin(); k!=results_by_signature.end(); ++k, ++group_id) {
		
		k->second.label_group_id(group_id) ;
		
		if (k->second.size() == 1) {
#if TEPAPA_MULTITHREAD 
			rr_out.push_back_safe( k->second.front() );
#else 
			rr_out.push_back( k->second.front() );
#endif // TEPAPA_MULTITHREAD
			continue;
			}
	
		string group_signature = get_group_signature(k->first) ;
	
#if TEPAPA_MULTITHREAD 
		tm.queue();
		tm.push_back( std::async( std::launch::async, &collapse_subpatterns, std::ref(group_signature), group_id, std::ref(rr_out), std::ref(k->second), std::ref(sl) ) );
#else 
		collapse_subpatterns(group_signature , group_id, rr_out, k->second, sl);
#endif // TEPAPA_MULTITHREAD
	
		}
	
#if TEPAPA_MULTITHREAD 
	tm.join_all();
#endif // TEPAPA_MULTITHREAD
		
	return 1;
	}


template <typename T> int reduce_by_collapse_subpatterns(TEPAPA_Results& rr_out, TEPAPA_Results& rr, const sample_list& sl) {
	
	rr_out.clear();
	rr_out.reserve( rr.size() );
	
	map<T, TEPAPA_Results>  results_by_signature = rr.group_by_signature<T>();
	
	return reduce_by_collapse_subpatterns(rr_out, results_by_signature, sl) ;
	}

	


template <typename T> int reduce_by_collapse_subpatterns(TEPAPA_Results& rr_out, TEPAPA_Results& rr) {
	
	rr_out.clear();
	
	map<T, TEPAPA_Results>  results_by_signature = rr.group_by_signature<T>();
	typename map<T, TEPAPA_Results> ::iterator  k;
	
	int group_id =1;
	
	for(k=results_by_signature.begin(); k!=results_by_signature.end(); ++k, ++group_id) {
		
		k->second.label_group_id(group_id) ;
		
		if (k->second.size() == 1) {
			rr_out.push_back( k->second.front() );
			continue;
			}
		
		TEPAPA_Results v, nv;
		
		split_mergeable_subsets(v, nv, k->second);
		
		rr_out.append( v.collapse_subpatterns() );
		rr_out.append( nv );
		}
	
	return 1;
	}
