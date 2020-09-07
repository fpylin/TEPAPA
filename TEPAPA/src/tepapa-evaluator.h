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
#ifndef __tepapa_evaluator_h
#define __tepapa_evaluator_h 1

#include "iptr.h"
#include "hashtable.h"
#include "bprof.h"
#include "roc.h"
#include "pattern.h"
#include "samples.h"
#include "tepapa-results.h"

#if TEPAPA_MULTITHREAD
#include <thread>
#include <mutex>
#endif // TEPAPA_MULTITHREAD


typedef  map< pair<hash_value, string>, TEPAPA_Result >   stats_cache_struct    ;


struct outvar_vectors_t {
  const vector<double>* p_v_score;
  const vector<bool>*   p_v_censored;

  outvar_vectors_t(){ p_v_score=0; p_v_censored=0; }
  
  outvar_vectors_t(const vector<double>& v_score){
    p_v_score = &v_score;
    p_v_censored = 0;
  }
  
  outvar_vectors_t(const vector<double>& v_score, const vector<bool>& v_censored){
    p_v_score = &v_score;
    p_v_censored = &v_censored;
  }  
};


class TEPAPA_Evaluator: public __iptr_member {
	stats_cache_struct    stats_cache;
	TEPAPA_Results        results;
	bool                  use_lrp;
#if TEPAPA_MULTITHREAD
	std::mutex  evaluator_mutex;
#endif // TEPAPA_MULTITHREAD
	
	int outvar_type;
	
	void eval_auroc(const binary_profile& vb, const vector<double>& v_score, iptr<pattern>& ppatt, double* p_pval_out=0);
	void eval_fisher_exact(const binary_profile& vb, const vector<double>& v_score, iptr<pattern>& ppatt, double* p_pval_out=0);
	void eval_logrank(const binary_profile& vb, const vector<double>& v_score, const vector<bool>& v_censored, iptr<pattern>& ppatt, double* p_pval_out=0);
	bool eval_spearman(const binary_profile& mask, const vector<double>& v_score, const vector<double>& v_predictors, iptr<pattern>& ppatt, double* p_pval_out=0);
	bool eval_auroc_rev(const binary_profile& mask, const vector<double>& v_score, const vector<double>& v_predictors, iptr<pattern>& ppatt, double* p_pval_out=0);

	public:
	//	TEPAPA_Evaluator(int p_outvar_type){ 
	//	  outvar_type = p_outvar_type;
	//	}

	TEPAPA_Evaluator(const sample_list& sl){ 
	  outvar_type = sl.get_outvar_type();
	  use_lrp = false;
	}

	void set_use_lrp (bool p_use_lrp) { use_lrp = p_use_lrp; }
	
	virtual ~TEPAPA_Evaluator(){}

	void eval_symbolic(const binary_profile& vb, const outvar_vectors_t& ovt, iptr<pattern>& ppatt, double* p_pval_out=0);
	
	bool eval_numeric(const binary_profile& mask, const outvar_vectors_t& ovt, const vector<double>& v_predictors, iptr<pattern>& ppatt, double* p_pval_out=0);

	// The following function is for matching partial sample set
	void eval_symbolic(const binary_profile& vb, const outvar_vectors_t& ovt, const binary_profile& mask, iptr<pattern>& ppatt, double* p_pval_out=0);	

	void reset() { results.clear(); }
	const TEPAPA_Results&  get_results() { return results; }
	};


#endif // __tepapa_evaluator_h
