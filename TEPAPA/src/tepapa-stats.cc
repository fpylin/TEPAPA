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
#include "tepapa-stats.h"
#include "stats.h"
#include "strfriends.h"

bool TEPAPA_Program_Statistic::handle_argv(const vector<string>& args) {
	for(unsigned int i=0; i<args.size(); ++i ) {
		variables.push_back( try_expand_variable( args[i]) );
		}
	return true;
	}
	
TEPAPA_Program_Statistic::TEPAPA_Program_Statistic() :
	TEPAPA_Program ("@Statistic") {
	options_optarg.push_back( 
		TEPAPA_option_optarg ( "-f", "--function", "", 's', &func )
		);
	}

int TEPAPA_Program_Statistic::run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) {
	TEPAPA_global_data_struct& gvr = *gvr_ptr;

	for(unsigned int i=0; i<variables.size(); ++i) {
		
		string s = gvr.variables[ variables[i] ] ;
		vector<double> v;
		
		int n_na = 0;
		
		do 	{
			size_t z = s.find ('\t');
			string ss = s.substr(0, z);
			if ( ss == "NA" ) {
				n_na ++;
				}
			else {
				v.push_back( atof(ss.c_str()) );
				}
			s = s.substr(z+1);
			}
		while( s.length() > 0 );
		
		s += strfmt("%s", variables[i].c_str());
		s += strfmt("\t%s\t%.0f", "N", double( v.size() + n_na ) );
		s += strfmt("\t%s\t%.0f", "NA", double( n_na ) );
		if ( v.size() ) {
			s += strfmt("\t%s\t%f", "MEAN", mean(v));
			s += strfmt("\t%s\t%f", "SD", sd(v));
			s += strfmt("\t%s\t%f", "MEDIAN", median(v));
			}
		printf("%s\n", s.c_str());
		}
	return TEPAPA_RETVAL_SUCCESS | TEPAPA_RETVAL_TERMINUS;
	}
