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

#include "tepapa-mlpredict.h"
#include "tepapa-msg.h"
#include "utils.h"
#include "strfriends.h"
#include "tepapa-utils.h"
#include "eperl.h"
#include "pipe.h"
#include <stdlib.h>
#include <unistd.h>


TEPAPA_Program_MLPRedict::TEPAPA_Program_MLPRedict()
	: TEPAPA_Program("@MLPredict") {
	
	options_binary.push_back(
		TEPAPA_option_binary("-a", "--use-WEKA-ARFF", "", 'b', &f_use_arff)
		);

	options_optarg.push_back(
		TEPAPA_option_optarg("-tr", "--training-set", "", 's', &fn_training)
		);

	options_optarg.push_back(
		TEPAPA_option_optarg("-ts", "--testing-set", "", 's', &fn_testing)
		);

	}


bool TEPAPA_Program_MLPRedict::handle_argv(const vector<string>& argv) {
	
	msgf(VL_INFO, "prog_path=%s\n", prog_path.c_str());

	EmbeddedPerl  eperl(true);
	
	string prog_dir = eperl.simple_replacement(prog_path, "(.*)/.*", "$1");
	
	msgf(VL_INFO, "prog_dir=%s\n", prog_dir.c_str());

	ext_classifiers.clear();
	
	for(unsigned int i=0; i<argv.size(); ++i) {
		string s = argv[i];
		string s_lc = strtolower(s);
		string s_uc = strtoupper(s);
		string sname = eperl.simple_replacement(s_lc, "[[:punct:]]+", "");
		
		mlcmd_struct  mcs; 
		
		mcs.cmdname = s;
		
		string cmdfn = find_executable(
			{ s, s_lc, s_uc },                                        // program name variations
			{ "./", prog_dir + "/", prog_dir + "/../scripts/", "" },  // directories
			{ "ml-", "" },                                            // program name prefixes
			{ ".pl", ".sh", ".py", "" }                               // program name suffixes
			);		
		
		ext_classifiers.push_back(mcs);
		
// 		18722022649
		
		msgf( VL_INFO, "name = %s\tcmdfn = %s\n", s.c_str(), mcs.cmdfn.c_str() );
		};
	
	return true;
	}



int TEPAPA_Program_MLPRedict::process_retval(int i, const string& s) {
	
	TEPAPA_global_data_struct& gvr = *gvr_ptr;
	
	size_t y, z=0, w = 0;
	
	int cnt = 0;
	do	{
// 		fprintf(stderr, "%s\n----\n", s.substr(z).c_str() );
		
		y = s.find('\n', z);
		string l = s.substr( z, ( (y == string::npos) ? string::npos : (y-z) ) );
// 		fprintf(stderr, ">> %d, %s\n----\n", l.size(), l.c_str() );
		
		if ( l.size() == 0 ) break;
		
		w = l.find('\t');
		if (w == string::npos) {
			msgf(VL_WARNING, "Line not parsed: %s\n", l.c_str());
			continue;
			}
			
		string f = l.substr(0,w);
		string v = l.substr(w+1);
		
		string fname = ext_classifiers[i].cmdname + string(".") + f;
		
		gvr.variables[ fname ] += v + string("\t");
		
		++cnt ;
		
		z  = y + 1;
// 		fprintf(stderr, "!%d\t%d\n", z, y);
		}
	while ( y != string::npos );
	
	return cnt;
	}


int TEPAPA_Program_MLPRedict::run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) {
		
	TEPAPA_global_data_struct& gvr = *gvr_ptr;
	
	vector<string> args ;
	
	if ( fn_training.length() + fn_testing.length() > 0 ) { // use custom filename 
		args.push_back( fn_training );
		args.push_back( fn_testing );
		}
	else if (f_use_arff) {
		args.push_back( gvr.variables[ GVAR_ACTIVE_DATASET_PREDICTIONS_TRAINING_ARFF ] );
		args.push_back( gvr.variables[ GVAR_ACTIVE_DATASET_PREDICTIONS_TESTING_ARFF ] );
		}
	else {
		args.push_back( gvr.variables[ GVAR_ACTIVE_DATASET_PREDICTIONS_TRAINING_CSV ] );
		args.push_back( gvr.variables[ GVAR_ACTIVE_DATASET_PREDICTIONS_TESTING_CSV ] );
		}
	
	int cnt = 0;
	
	for (unsigned int i=0; i<ext_classifiers.size(); ++i) {
		
		msgf( VL_INFO, "%d\t%d\t%s %s\n", cnt, getpid(), args[0].c_str(), args[1].c_str() );

		string retval = pipe_command(ext_classifiers[i].cmdfn, args, "") ;
	
		printf ( "%s\n", retval.c_str() );
		
		process_retval(i, retval);
		
		++cnt;
		}
		
	return TEPAPA_RETVAL_SUCCESS;
	}
