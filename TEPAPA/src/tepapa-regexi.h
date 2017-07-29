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

#ifndef __tepapa_regexi_h
#define __tepapa_regexi_h 1


#include "tepapa.h"
#include "tepapa-gvar.h"
#include "tepapa-program.h"

class TEPAPA_Program_RegexInducer: public TEPAPA_Program {
	bool f_by_subgroups ;
	bool f_recursive_reduction;

	public:
	TEPAPA_Program_RegexInducer();

	virtual int run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) ;
	
	virtual TEPAPA_Program* spawn() const { return new TEPAPA_Program_RegexInducer(); }
	};




// TAKE 2: This is a more sophisticated version than InduceRegex

#include "tepapa-discoverer.h"

#define TEPAPA_ARGSTR_DISCOVERER_REGEX_BY_SUBGROUPS         "-g"
#define TEPAPA_ARGSTR_DISCOVERER_REGEX_RECURSIVE_REDUCTION  "-r"
#define TEPAPA_ARGSTR_DISCOVERER_REGEX_GREEDY_HILLCLIMB     "-c"


class TEPAPA_Discoverer_Regex: public TEPAPA_Discoverer {

	protected:
	
	std::mutex  discoverer_regex_mutex;
	
	bool f_by_subgroups ;
	bool f_recursive_reduction ;
	bool f_greedy_hillclimb ;
	
	bool eval_regex(iptr<TEPAPA_Evaluator>& e, const sample_list& sl, regex& r, double* ppval = 0); // returns p value
	bool eval_regex(iptr<TEPAPA_Evaluator>& e, const sample_list& sl, regex& r, binary_profile& search_mask, double* ppval = 0); // returns p value
	void recursive_reduce_exhaustive(iptr<TEPAPA_Evaluator>& e, const sample_list& sl, regex& r, unsigned int cur, binary_profile& search_mask, regex& best_regex, double& best_pval) ;
	
	regex recursive_reduce_greedy(const sample_list& sl, regex& r0) ;
	
	public:
	virtual bool is_meta_learner() const { return true; }
	
	TEPAPA_Discoverer_Regex(VariableList& param): TEPAPA_Discoverer(param) {
		f_by_subgroups = int( param[TEPAPA_ARGSTR_DISCOVERER_REGEX_BY_SUBGROUPS] );
		f_recursive_reduction = int( param[TEPAPA_ARGSTR_DISCOVERER_REGEX_RECURSIVE_REDUCTION] );
		f_greedy_hillclimb = int( param[TEPAPA_ARGSTR_DISCOVERER_REGEX_GREEDY_HILLCLIMB] );
		}

	virtual ~TEPAPA_Discoverer_Regex() {}
	virtual void run1(int) final {}
	virtual bool run1_meta(const sample_list& sl, const TEPAPA_Results& rr_input);
	};


class TEPAPA_Program_Discoverer_Regex: public TEPAPA_Program_Discoverer<TEPAPA_Discoverer_Regex> {
	public:
	TEPAPA_Program_Discoverer_Regex();
	virtual TEPAPA_Program* spawn() const { return new TEPAPA_Program_Discoverer_Regex; }
	};


#endif
