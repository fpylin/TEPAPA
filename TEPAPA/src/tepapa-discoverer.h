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
#ifndef __tepapa_discoverer_h
#define __tepapa_discoverer_h 1

#include "hashtable.h"
#include "ngram-registry.h"
#include "samples.h"

extern set<hash_value> separators;

#include "regex.h"
#include "tepapa-evaluator.h"
#include "tepapa-gvar.h"

// vector<regex> load_regices(const char* filename);


class TEPAPA_Discoverer {
	protected:
	ngram_registry          hr;
	
	sample_list*            sl_ptr;
	vector<double>          v_score;
	iptr<TEPAPA_Evaluator>  evaluator;
	VariableList*           p_param;
	
	int    total_cnt_cls;
	
	virtual void run1(int i) =0;
	virtual bool run1_meta(const sample_list& sl, const TEPAPA_Results& rr_input) { // used by meta-learners
		return false;
		} 
	
	virtual void initialise(void) {};

	public:
	virtual bool is_meta_learner() const { return false; }
	void set_params(VariableList&  param_ref) { p_param = &param_ref; }
		
	TEPAPA_Discoverer();
	TEPAPA_Discoverer(VariableList& param_ref) ;

	virtual ~TEPAPA_Discoverer() {}

	void set_sample_list(sample_list&  p_sl_ptr) ;
	
	virtual bool run() ;  // de novo learning
	virtual bool run(TEPAPA_Results&  rr) ;  // meta-learner, default return = false
	
	virtual const TEPAPA_Results& get_results() const { return evaluator-> get_results() ; }
	} ;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "tepapa-program.h"

template <typename DISCOVERER_CLASS> class TEPAPA_Program_Discoverer: public TEPAPA_Program {
	protected:
// 	bool f_do_tepapa_binary ;
// 	bool f_deep_cmp  ;
// 	bool f_wildcard ;
// 	int  ngram ;
	
	public:
	TEPAPA_Program_Discoverer(const string& pname);

	virtual int run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) ;
	};



#include "tepapa.h"
#include "tepapa-discoverer.h"
#include "tepapa-msg.h"
#include "tepapa-gvar.h"

template <typename DISCOVERER_CLASS> int TEPAPA_Program_Discoverer<DISCOVERER_CLASS>::run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) { 

	ngram_registry hr(ght);

	TEPAPA_Discoverer*  discoverer = new DISCOVERER_CLASS(param);

	if ( ! discoverer ) {
		msgf(VL_FATAL, "Unable to create discoverer");
		return false;
		}

	discoverer -> set_sample_list( ds_input.sl );
	
	if ( discoverer -> is_meta_learner() ) {
		discoverer -> run(ds_input.rr);
		}
	else {
		discoverer -> run();
		}
	
	TEPAPA_Results res = discoverer->get_results();
	
	rr_output = res; 

	delete discoverer;
	
	return TEPAPA_RETVAL_SUCCESS;
	}


#define  TEPAPA_ARGSTR_DISCOVERER_USE_WILDCARD       "-w"
#define  TEPAPA_ARGSTR_DISCOVERER_USE_DEEP_CMP       "-d"
#define  TEPAPA_ARGSTR_DISCOVERER_MAX_NGRAM          "-n"
#define  TEPAPA_ARGSTR_DISCOVERER_POSITIVE_ONLY      "-p"

template <typename DISCOVERER_CLASS> TEPAPA_Program_Discoverer<DISCOVERER_CLASS>::TEPAPA_Program_Discoverer(const string& pname)
	: TEPAPA_Program(pname) {
// 	f_do_tepapa_binary = false;
// 	f_deep_cmp = false;
// 	f_wildcard = false;
//	ngram = 100;
	param[TEPAPA_ARGSTR_DISCOVERER_MAX_NGRAM] = 100;
	
	options_binary.push_back(
		TEPAPA_option_binary(TEPAPA_ARGSTR_DISCOVERER_USE_WILDCARD, "--use-wildcard", "", 'b')
		);
	
	options_binary.push_back(
		TEPAPA_option_binary(TEPAPA_ARGSTR_DISCOVERER_USE_DEEP_CMP, "--use-deep-comparison", "", 'b') 
		);
	
	options_binary.push_back( 
		TEPAPA_option_binary(TEPAPA_ARGSTR_DISCOVERER_POSITIVE_ONLY, "--positive-pattern-only", "", 'b')
		); 

	options_optarg.push_back( 
		TEPAPA_option_optarg(TEPAPA_ARGSTR_DISCOVERER_MAX_NGRAM, "--max-ngram", "", 'i')
		); 
	
	}


#endif // __tepapa_discoverer_h 
