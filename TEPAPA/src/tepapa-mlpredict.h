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

#ifndef __tepapa_mlpredict_h
#define __tepapa_mlpredict_h 1

#include "tepapa-program.h"

using namespace std;

#include <vector>
#include <string>

struct mlcmd_struct {
	string   cmdname ;
	string   cmdfn ; 
	};

class TEPAPA_Program_MLPRedict: public TEPAPA_Program {
	
	vector<mlcmd_struct>  ext_classifiers;
	bool f_use_arff;
	
	string fn_training;
	string fn_testing;
	
	protected: 
	virtual bool handle_argv(const vector<string>& argv) ;
	int process_retval(int i, const string& s) ;
	
	public:
	TEPAPA_Program_MLPRedict();

	virtual int run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) ;
	virtual TEPAPA_Program* spawn() const { return new TEPAPA_Program_MLPRedict; }
	};



#endif //  __tepapa_mlpredict_h
