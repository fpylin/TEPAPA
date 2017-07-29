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
#ifndef __tepapa_sumpatt_h
#define __tepapa_sumpatt_h 1

#include "tepapa-program.h"
#include "tepapa-gvar.h"

class TEPAPA_Program_Pattern_Summariser: public TEPAPA_Program {
	int    n_iters_base;     // base number of iterations (to calculate the denominator for average)
	double f_cut       ;     // only show occurs in > f_cut of samples
	bool   f_export_markers; // export as informarkers
	vector<string>  ds_comparison; // dataset for comparison
	
	public:
	TEPAPA_Program_Pattern_Summariser();
	
	virtual int run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) ;
	
	bool handle_argv(const vector<string>& argv);
	
	virtual TEPAPA_Program* spawn() const { return new TEPAPA_Program_Pattern_Summariser; }
	};

#endif //  __tepapa_sumpatt_h 1
