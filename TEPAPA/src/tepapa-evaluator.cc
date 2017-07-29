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
#include "tepapa-evaluator.h"
#include "tepapa-utils.h"
#include "strfriends.h"

#include <algorithm>



void TEPAPA_Evaluator::eval_auroc(const binary_profile& vb, const vector<double>& v_score, iptr<pattern>& ppatt, double* p_pval_out) {
	string vbhex = vb.digest();

	TEPAPA_Result  r;
	r.bprof = vb;
	r.patt = ppatt;
	r.method = ght("AUC");
	
#if TEPAPA_MULTITHREAD
	evaluator_mutex.lock();
#endif // TEPAPA_MULTITHREAD	
	map<string, TEPAPA_Result >::const_iterator  stci = stats_cache.find( vbhex ) ;
	
	if ( stci != stats_cache.end() ) {
		r.est  = stci->second.est;
		r.pval = stci->second.pval;
		r.npos = stci->second.npos;
		r.dir  = stci->second.dir;
		
#if TEPAPA_MULTITHREAD
		evaluator_mutex.unlock();
#endif // TEPAPA_MULTITHREAD		
		}
	else 
		{
#if TEPAPA_MULTITHREAD
		evaluator_mutex.unlock();
#endif // TEPAPA_MULTITHREAD
		
		roc_curve roc;
		roc.compute( vb, v_score );
		r.pval = 1.00;
		r.est  = roc.AUC(& (r.pval) );
		r.npos = vb.npos();
		r.dir  = ( (r.est > 0.5) ? 1 : ( (r.est < 0.5) ? -1 : 0) );
#if TEPAPA_MULTITHREAD
		evaluator_mutex.lock();
#endif // TEPAPA_MULTITHREAD	
		stats_cache[ vbhex.c_str() ] = r;
#if TEPAPA_MULTITHREAD
		evaluator_mutex.unlock();
#endif // TEPAPA_MULTITHREAD
		}
	

#if TEPAPA_MULTITHREAD
	std::lock_guard<std::mutex>  lock(evaluator_mutex);
#endif // TEPAPA_MULTITHREAD
	results.push_back(r);
	if (p_pval_out) *p_pval_out = r.pval;
	
// 	printf("%s", r.log_string().c_str());
	}



void TEPAPA_Evaluator::eval_fisher_exact(const binary_profile& vb, const vector<double>& v_score, iptr<pattern>& ppatt, double* p_pval_out) {
	string vbhex = vb.digest();
	
	TEPAPA_Result r ;
	r.bprof = vb;
	r.patt = ppatt;
	r.method = ght("LOR.FET");
// 	r.hv = ppatt->get_uniq_id(); // ght( ppatt->to_string().c_str() );
	
#if TEPAPA_MULTITHREAD
	evaluator_mutex.lock();
#endif // TEPAPA_MULTITHREAD	
	map<string, TEPAPA_Result >::const_iterator  stci = stats_cache.find(vbhex) ;
	
	if ( stci != stats_cache.end() ) {
		r.est  = stci->second.est;
		r.pval = stci->second.pval;
		r.npos = stci->second.npos;
		r.dir  = stci->second.dir;
#if TEPAPA_MULTITHREAD
		evaluator_mutex.unlock();
#endif // TEPAPA_MULTITHREAD		
		}
	else
		{
#if TEPAPA_MULTITHREAD
		evaluator_mutex.unlock();
#endif // TEPAPA_MULTITHREAD			
		two_class_stats tcs;
		tcs.compute( vb, v_score );
// 		tcs.print("abcdefgh");
		r.est  = tcs.log_or();
		r.pval = tcs.fisher_pval();
		r.npos = vb.npos();
		r.dir  = ( (r.est > 0) ? 1 : ( (r.est < 0) ? -1 : 0) );
#if TEPAPA_MULTITHREAD
		evaluator_mutex.lock();
#endif // TEPAPA_MULTITHREAD
		stats_cache[ vbhex.c_str() ] = r;
#if TEPAPA_MULTITHREAD
		evaluator_mutex.unlock();
#endif // TEPAPA_MULTITHREAD		
		}

#if TEPAPA_MULTITHREAD
	std::lock_guard<std::mutex>  lock(evaluator_mutex);
#endif // TEPAPA_MULTITHREAD
	results.push_back(r);
	if (p_pval_out) *p_pval_out = r.pval;

// 	printf("%s", r.log_string("%-10.4g").c_str());
	}



bool TEPAPA_Evaluator::eval_spearman(const binary_profile& bp_mask, const vector<double>& v_score, const vector<double>& v_predictors, iptr<pattern>& ppatt, double* p_pval_out) {

	double rho, pval;
	
	unsigned int npos = bp_mask.npos() ;
	
	if ( npos < (bp_mask.size() / 10)  ) return  false ;
	if ( npos <= 2 ) return false ;
	
	vector<double> extr_x = subset(v_predictors, bp_mask);
	vector<double> extr_s = subset(v_score, bp_mask);
	
	pval = 1;
	
	rho = spearman(extr_x, extr_s, &pval);
	
	if ( isnanf(rho) ) return false ;

	TEPAPA_Result r ;
	r.bprof  = bp_mask;
	r.patt   = ppatt;
	r.method = ght("SPEARMAN");
// 	r.hv     = ppatt->get_uniq_id(); // ght( ppatt->to_string().c_str() );
	r.est    = rho;
	r.pval   = pval;
	r.npos   = npos;
	r.val    = v_predictors;
	r.dir  = ( (r.est > 0) ? 1 : ((r.est < 0) ? -1 : 0) );
	
	
#if TEPAPA_MULTITHREAD
	std::lock_guard<std::mutex>  lock(evaluator_mutex);
#endif // TEPAPA_MULTITHREAD
	results.push_back(r);
	if (p_pval_out) *p_pval_out = r.pval;
	
	return true ;
	}




bool TEPAPA_Evaluator::eval_auroc_rev(const binary_profile& bp_mask, const vector<double>& v_score, const vector<double>& v_predictors, iptr<pattern>& ppatt, double* p_pval_out) {

	vector<double> extr_x = subset(v_predictors, bp_mask);
	vector<double> extr_s = subset(v_score, bp_mask);
	
	int npos = bp_mask.npos(); 
	
	if (extr_x.size() <= 2) return false;
	if (extr_s.size() <= 2) return false;
	
	roc_curve roc;
	roc.compute( extr_s , extr_x );

	TEPAPA_Result r;
	r.bprof  = bp_mask;
	r.patt   = ppatt;
	r.method = ght("AUROC2");
// 	r.hv     = ppatt->get_uniq_id(); // ght( ppatt->to_string().c_str() );
	r.pval   = 1.00;
	r.est    = roc.AUC(& (r.pval) );
	r.npos   = npos ;
	r.dir  = ( (r.est > 0.5) ? 1 : ( (r.est < 0.5) ? -1 : 0) );
	r.val    = v_predictors;
	if ( isnanf(r.pval) ) return false;
	
#if TEPAPA_MULTITHREAD
	std::lock_guard<std::mutex>  lock(evaluator_mutex);
#endif // TEPAPA_MULTITHREAD
	results.push_back(r);
	if (p_pval_out) *p_pval_out = r.pval;
	
	return true;
	}


void TEPAPA_Evaluator::eval_symbolic(const binary_profile& vb, const vector<double>& v_score, iptr<pattern>& ppatt, double* p_pval_out) {
	if (f_outvar_is_binary) {
		eval_fisher_exact(vb, v_score, ppatt, p_pval_out);
		}
	else {
		eval_auroc(vb, v_score, ppatt, p_pval_out);
		}
	}

bool TEPAPA_Evaluator::eval_numeric(const binary_profile& mask, const vector<double>& v_score, const vector<double>& v_predictors, iptr<pattern>& ppatt, double* p_pval_out) {
	if (f_outvar_is_binary) {
		return eval_auroc_rev(mask, v_score, v_predictors, ppatt, p_pval_out);
		}
	else {
		return eval_spearman(mask, v_score, v_predictors, ppatt, p_pval_out);
		}
	}
