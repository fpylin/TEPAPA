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
#include "tepapa-discoverer.h"
#include "tepapa-program.h"
#include "tepapa-evaluator.h"

#define TEPAPA_ARGSTR_DISCOVERER_META_PATTERN_DEPTH  "-r"

struct MPL_retval_struct {
	iptr<meta_pattern>   mp;
	binary_profile       bprof_out;
	};

class TEPAPA_Discoverer_MetaPattern: public TEPAPA_Discoverer {
	
	int depth;
	
	MPL_retval_struct MPL_AND(const TEPAPA_Result& r1, const TEPAPA_Result& r2);
	MPL_retval_struct MPL_OR(const TEPAPA_Result& r1, const TEPAPA_Result& r2);
	MPL_retval_struct MPL_NOT(const TEPAPA_Result& r1);
	MPL_retval_struct MPL_FOLLOW(const sample_list& sl, const TEPAPA_Result& r1, const TEPAPA_Result& r2) ;
	
	bool do_evaluate(MPL_retval_struct& pat_match, const vector<double>& v_score, bool f_is_binary);

	protected:

	virtual bool is_meta_learner() const { return true; }
	
	void set_v_score(const sample_list& sl);
	
	bool do_evaluate(const MPL_retval_struct& pat_match) ;

	public:
	TEPAPA_Discoverer_MetaPattern(VariableList& param): TEPAPA_Discoverer(param) {
		depth = param[TEPAPA_ARGSTR_DISCOVERER_META_PATTERN_DEPTH];
		}

	virtual void run1(int) { printf("run1\n"); }
	virtual bool run1_meta(const sample_list& sl, const TEPAPA_Results& rr_input);
	bool gen_MPL_AND_recursive(const TEPAPA_Results& rr, const vector<unsigned int>& v, const vector<bool>& v_neg, const binary_profile& bprof, unsigned int limit, unsigned int delta) ;
	bool gen_MPL_FOLLOW_recursive(const sample_list& sl, const TEPAPA_Results& rr, const vector<unsigned int>& v, const vector<size_t>& sli, const binary_profile& bprof, unsigned int limit);
	  
	};


class TEPAPA_Program_Discoverer_MetaPattern: public TEPAPA_Program_Discoverer<TEPAPA_Discoverer_MetaPattern> {
	public:
	TEPAPA_Program_Discoverer_MetaPattern();
	virtual TEPAPA_Program* spawn() const { return new TEPAPA_Program_Discoverer_MetaPattern; }
	};




#endif // __tepapa_meta_h 
