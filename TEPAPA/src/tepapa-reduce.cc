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

#include "tepapa-reduce.h"
#include "tepapa-msg.h"

#include "samples.h"
#include "fgroup.h"

TEPAPA_Program_Feature_Grouper::TEPAPA_Program_Feature_Grouper()
	: TEPAPA_Program("@GroupFeatures") {
	
	options_optarg.push_back(
		TEPAPA_option_optarg("-f", "--format", "", 's', &fmt)
		);
	}

	
int TEPAPA_Program_Feature_Grouper::run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) {
	
	TEPAPA_Results  rr = ds_input.rr;
	
	rr.label_by_signature(fmt);
	rr_output = rr;
	
	return TEPAPA_RETVAL_SUCCESS;
	}


TEPAPA_Program_Feature_Reducer::TEPAPA_Program_Feature_Reducer()
	: TEPAPA_Program("@ReduceFeatures") {
	
// 	options_binary.push_back( 
// 		TEPAPA_option_binary("-s", "--collapse-subpatterns-only", "", 'b', &f_subpattern_only)
// 		);
	}

	
int TEPAPA_Program_Feature_Reducer::run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) {
	
	TEPAPA_Results  rr = ds_input.rr;
	
	map<int, TEPAPA_Results>  m = rr.group_by_signature<int>();
	
	reduce_by_collapse_subpatterns(rr_output, m, ds_input.sl) ;
	
	return TEPAPA_RETVAL_SUCCESS;
	}
