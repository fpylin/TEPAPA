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

#ifndef __tepapa_main_h
#define __tepapa_main_h 1

#include <vector>
#include "tepapa-program.h"
#include "tepapa-command.h"
#include "tepapa-gvar.h"


using namespace std;

class TEPAPA_Program_Main: public TEPAPA_Program {
	bool f_do_help;
	bool f_quiet;
	int  verbosity_level;

	void title (void) const;
	
	const program_list*  programs_ptr;
	const command_list*  commands_ptr;

	public:
	TEPAPA_Program_Main();
	
	virtual bool handle_argv(const vector<string>& argv) ;

	virtual int run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) ;
	
	virtual int run(TEPAPA_global_data_struct& gvr, command_list& commands, const program_list&  programs) final ;
	
	virtual void initialise(void);
	
	virtual TEPAPA_Program* spawn() const { return new TEPAPA_Program_Main; }
	};

#endif // __tepapa_main_h
