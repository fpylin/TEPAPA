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
#ifndef __tepapa_tag_h
#define __tepapa_tag_h  1

#include "tepapa-program.h"
#include <string>
#include <vector>

class TEPAPA_Program_TagFile: public TEPAPA_Program {
	vector<double> score_list;
	vector<string> file_list;
	string path_case_raw;
	string path_output;
	
	protected: 
	virtual bool handle_argv(const vector<string>& argv) ;

	public:
	TEPAPA_Program_TagFile();

	virtual int run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) ;
	virtual TEPAPA_Program* spawn() const { return new TEPAPA_Program_TagFile; }
	};


#endif // __tepapa_tag_h
