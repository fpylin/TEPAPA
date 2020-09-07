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
#include <unistd.h>
#include "tepapa.h"
#include "tepapa-discoverer.h"

#include "tepapa-msg.h"
#include "parallel.h"

// vector<regex> load_regices(const char* filename) {
// // 	int n = 0;
// 	FILE* fin = fopen( filename, "rt"); 
// 	char buf [1048576];
// 	vector<regex>  retval;
// 	while( fgets(buf, sizeof(buf), fin) ) {
// 		regex  re;
// 		char* z = parse_regex_xml(re, ght, buf);
// 		if(z) retval.push_back(re);
// 		}
// 	fclose(fin);
// 	return retval;
// 	}


TEPAPA_Discoverer::TEPAPA_Discoverer() {
	hr.set_hash_table(ght);
	sl_ptr = 0;
	}

TEPAPA_Discoverer::TEPAPA_Discoverer(VariableList& param_ref) {
	hr.set_hash_table(ght);
	sl_ptr = 0;
	
	set_params(param_ref);
	}


void TEPAPA_Discoverer::set_sample_list(const sample_list& sl_ref) { 
	sl_ptr = &sl_ref;
	
	const sample_list& sl = *sl_ptr;
	
	v_score = sl.get_scores(); 
	v_censored = sl.get_censored();       
	}


bool TEPAPA_Discoverer::run() {
	const sample_list& sl = *sl_ptr;
	VariableList& param = *(p_param);

	evaluator= new TEPAPA_Evaluator( sl );
	
	stopwatch  sw;
	
	sw.start();
	
	initialise();
	
	total_cnt_cls  = 0;

	bool f_positive_only = int( param[TEPAPA_ARGSTR_DISCOVERER_POSITIVE_ONLY] );
	bool f_use_lrp = int( param[TEPAPA_ARGSTR_DISCOVERER_USE_LIKELIHOOD_RATIO] );
	min_support = int( param[TEPAPA_ARGSTR_DISCOVERER_MIN_SUPPORT] );

	msgf(VL_NOTICE, "min_support = %d cases\n", min_support);
	
	evaluator->set_use_lrp(f_use_lrp);

	
#if TEPAPA_MULTITHREAD 
	thread_manager  tm;
#endif // TEPAPA_MULTITHREAD
	
	for(unsigned long int i=0; i<sl.size(); ++i) {
		double f_completed = ( double(i) + 0.5 ) / double(sl.size()) * 100;
// 		double time_rem_sec = sw.eta_sec( f_completed );
		if ( sl.is_outvar_binary() && f_positive_only && v_score[i] == 0 ) continue;
			
		msgf(VL_NOTICE, "Sample %4lu/%4lu  [%8.3f sec]  (%4.1f%% %8lu results) %.6f %s\n", 
			i+1, sl.size(), evaluator->get_results().size(), sw.elapsed_sec(), f_completed,
// 			sw.get_eta_str(f_completed).c_str(), time_rem_sec, // ETA %s (%6.1fs)  
			v_score[i], ght[ sl[i].definitions[0] ]
			);

#if TEPAPA_MULTITHREAD 
		tm.queue();
		tm.push_back( std::async( std::launch::async, &TEPAPA_Discoverer::run1, this, i )  );
#else 
		run1(i);
#endif // TEPAPA_MULTITHREAD
		}
	
#if TEPAPA_MULTITHREAD 
	tm.join_all();
#endif // TEPAPA_MULTITHREAD

	msgf(VL_NOTICE, "Total = %lu results\n", total_cnt_cls);

	return (total_cnt_cls != 0 ? true : false) ;
	}



///////////////////////////////////////////////////////////

bool TEPAPA_Discoverer::run(TEPAPA_Results&  rr) {  // meta learner
	const sample_list& sl = *sl_ptr;

	VariableList& param = *(p_param);
	
	evaluator= new TEPAPA_Evaluator( sl );
	
	bool f_positive_only = int( param[TEPAPA_ARGSTR_DISCOVERER_POSITIVE_ONLY] );
	bool f_use_lrp = int( param[TEPAPA_ARGSTR_DISCOVERER_USE_LIKELIHOOD_RATIO] );
	min_support = int( param[TEPAPA_ARGSTR_DISCOVERER_MIN_SUPPORT] );

	printf("min_support = %d results\n", min_support);
	
	evaluator->set_use_lrp(f_use_lrp);

	stopwatch  sw;
	
	sw.start();
	
	initialise();
	
	total_cnt_cls  = 0;
	
	run1_meta(sl, rr);

	msgf(VL_NOTICE, "Total = %lu results\n", total_cnt_cls);
	
	return true;
	}

