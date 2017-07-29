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
#ifndef __tepapa_meta_h
#define __tepapa_meta_h 1 

#include "meta-pattern.h"

#include "tepapa-gvar.h"
#include "tepapa-results.h"
#include "tepapa-program.h"
#include "tepapa-evaluator.h"

struct MPL_retval_struct {
	meta_pattern*   mp;
	binary_profile  bprof_out;
	MPL_retval_struct(){ mp = 0; }
	};

class TEPAPA_Program_Meta_Pattern_Learner: public TEPAPA_Program {
	iptr<TEPAPA_Evaluator>  evaluator;
	
	int depth;
	bool f_gen_by_cooc_sig;
	
	MPL_retval_struct MPL_AND(const TEPAPA_Result& r1, const TEPAPA_Result& r2);
	MPL_retval_struct MPL_OR(const TEPAPA_Result& r1, const TEPAPA_Result& r2);
	MPL_retval_struct MPL_NOT(const TEPAPA_Result& r1);
	MPL_retval_struct MPL_FOLLOW(const sample_list& sl, const TEPAPA_Result& r1, const TEPAPA_Result& r2) ;
	
	bool do_evaluate(MPL_retval_struct& pat_match, const vector<double>& v_score, bool f_is_binary);

	int gen_meta_pattern_by_cooccurrence_signature(TEPAPA_Results& rr_out, const TEPAPA_Results& rr);
	
	protected:

	bool f_is_binary ;
	vector<double>  v_score;
	vector<string> fmts;
	
	void set_v_score(const sample_list& sl);
	
	bool do_evaluate(const MPL_retval_struct& pat_match) ;

	public:
	TEPAPA_Program_Meta_Pattern_Learner();

	virtual bool handle_argv(const vector<string>& argv) ;

	virtual int run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) ;
	virtual TEPAPA_Program* spawn() const { return new TEPAPA_Program_Meta_Pattern_Learner; }
	
	};

#endif // __tepapa_meta_h 
