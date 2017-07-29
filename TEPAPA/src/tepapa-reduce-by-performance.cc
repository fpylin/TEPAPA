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

#include "tepapa-reduce-by-performance.h"
#include "tepapa-msg.h"
#include "tepapa-utils.h"
#include "regex.h"
#include "meta-pattern.h"

#include "fgroup.h"

#include <algorithm>


TEPAPA_Program_Reduce_By_Performance::TEPAPA_Program_Reduce_By_Performance()
	: TEPAPA_Program("@ReduceByPerformance") {
	}

	
int TEPAPA_Program_Reduce_By_Performance::run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) {
	
	TEPAPA_Results  rr = ds_input.rr;

	reduce_by_collapse_subpatterns<pval_est_pair>(rr, rr, ds_input.sl);
	
	rr_output = rr;

	return TEPAPA_RETVAL_SUCCESS;
	}
