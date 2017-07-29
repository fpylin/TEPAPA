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
#include "tepapa-results.h"

#if TEPAPA_MULTITHREAD
#include <thread>
#include <mutex>
#endif // TEPAPA_MULTITHREAD


typedef  map<string, TEPAPA_Result >   stats_cache_struct    ;

class TEPAPA_Evaluator: public __iptr_member {
	stats_cache_struct    stats_cache;
	TEPAPA_Results        results;
#if TEPAPA_MULTITHREAD
	std::mutex  evaluator_mutex;
#endif // TEPAPA_MULTITHREAD
	
	bool f_outvar_is_binary ;
	
	void eval_auroc(const binary_profile& vb, const vector<double>& v_score, iptr<pattern>& ppatt, double* p_pval_out=0);
	void eval_fisher_exact(const binary_profile& vb, const vector<double>& v_score, iptr<pattern>& ppatt, double* p_pval_out=0);
	bool eval_spearman(const binary_profile& mask, const vector<double>& v_score, const vector<double>& v_predictors, iptr<pattern>& ppatt, double* p_pval_out=0);
	bool eval_auroc_rev(const binary_profile& mask, const vector<double>& v_score, const vector<double>& v_predictors, iptr<pattern>& ppatt, double* p_pval_out=0);

	public:
	TEPAPA_Evaluator(bool p_f_outvar_is_binary){ 
		f_outvar_is_binary = p_f_outvar_is_binary;
		}
	
	virtual ~TEPAPA_Evaluator(){}

	void eval_symbolic(const binary_profile& vb, const vector<double>& v_score, iptr<pattern>& ppatt, double* p_pval_out=0);
	bool eval_numeric(const binary_profile& mask, const vector<double>& v_score, const vector<double>& v_predictors, iptr<pattern>& ppatt, double* p_pval_out=0);

	void reset() { results.clear(); }
	const TEPAPA_Results&  get_results() { return results; }
	};


#endif // __tepapa_evaluator_h
