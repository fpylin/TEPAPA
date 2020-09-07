/* tepapa.cc -- TEPAPA main file
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

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <execinfo.h>
#include <signal.h>

#include "strfriends.h"

#include "tepapa.h"
#include "tepapa-main.h"
#include "tepapa-symbolic.h"
#include "tepapa-numeric.h"
#include "tepapa-clusterer.h"
#include "tepapa-selsig.h"
#include "tepapa-reduce-cooccurrence.h"
#include "tepapa-reduce-by-performance.h"
#include "tepapa-reduce-infsubset.h"
#include "tepapa-tester.h"
#include "tepapa-ranker.h"
#include "tepapa-io.h"
#include "tepapa-regexi.h"
#include "tepapa-resample.h"
#include "tepapa-loop.h"
#include "tepapa-mlpredict.h"
#include "tepapa-gvar.h"
#include "tepapa-msg.h"
#include "tepapa-preprocess.h"
#include "tepapa-bowsent.h"
#include "tepapa-sentprof.h"
#include "tepapa-sumpatt.h"
#include "tepapa-meta.h"
#include "tepapa-stats.h"
#include "tepapa-reduce.h"
#include "tepapa-tag.h"
#include "tepapa-gseq.h"

#include "predicate-pattern.h"
#include "tepapa-command.h"



std::string           sub_program = COMMAND_NAME_MAIN;
std::string           prog_path = "";
std::set<hash_value>  hv_separators;
hash_value            hv_wildcard;
hash_value            hv_class_NUMBER;


#include "stopwatch.h"

stopwatch  master_clock;

program_list  programs;

bool initialise(TEPAPA_global_data_struct& gvr) {
	ght ( "(null)" );
	hv_separators.insert( ght ( "<CRLF/>" ) );
	hv_separators.insert( ght ( "<SEP/>" ) );
	hv_wildcard	= ght("<TOKEN/>");
	hv_class_NUMBER = ght("<NUMBER/>");
// 	hv_class_NUMBER_wildcard = ght("<NUMBER/*>");
	
	gvr.set_active_dataset( DEFAULT_DATASET_NAME ) ;
	
	gvr.variables[ GVAR_ACTIVE_DATASET           ] =  DEFAULT_DATASET_NAME ;
	gvr.variables[ GVAR_ACTIVE_DATASET_TRAINING  ] =  DEFAULT_DATASET_NAME ;
	gvr.variables[ GVAR_ACTIVE_DATASET_TESTING   ] =  DEFAULT_DATASET_NAME ;

	gvr.variables[ GVAR_ACTIVE_DATASET_PREDICTIONS_ARFF ]          = "default.arff";
	gvr.variables[ GVAR_ACTIVE_DATASET_PREDICTIONS_TRAINING_ARFF ] = "default-tr.arff";
	gvr.variables[ GVAR_ACTIVE_DATASET_PREDICTIONS_TESTING_ARFF ]  = "default-ts.arff";
	gvr.variables[ GVAR_ACTIVE_DATASET_PREDICTIONS_CSV ]           = "default.csv";
	gvr.variables[ GVAR_ACTIVE_DATASET_PREDICTIONS_TRAINING_CSV ]  = "default-tr.csv";
	gvr.variables[ GVAR_ACTIVE_DATASET_PREDICTIONS_TESTING_CSV ]   = "default-ts.csv";

	gvr.variables[ "PID" ]   = strfmt("%u", getpid());
	
	master_clock.start();
	
	hypergeometric_log_i_cache_init();
	
	programs.insert( new TEPAPA_Program_Main );
	programs.insert( new TEPAPA_Program_Set );
	programs.insert( new TEPAPA_Program_MergeDatasets );
	programs.insert( new TEPAPA_Program_CopyDataset );
	programs.insert( new TEPAPA_Program_Transfer );
	programs.insert( new TEPAPA_Program_Preprocess );
	programs.insert( new TEPAPA_Program_LoadTable );
	programs.insert( new TEPAPA_Program_LoadCases );
	programs.insert( new TEPAPA_Program_LoadCharstring );
	programs.insert( new TEPAPA_Program_GenerateCases );
	programs.insert( new TEPAPA_Program_Discoverer_Symbolic );
	programs.insert( new TEPAPA_Program_Discoverer_Numeric );
	programs.insert( new TEPAPA_Program_SigSelect );
	programs.insert( new TEPAPA_Program_Reduce_By_Cooccurrence );
	programs.insert( new TEPAPA_Program_Reduce_By_Performance );
	programs.insert( new TEPAPA_Program_Reduce_Inferior_Subsets );
	programs.insert( new TEPAPA_Program_Feature_Grouper );
	programs.insert( new TEPAPA_Program_Feature_Reducer );
	programs.insert( new TEPAPA_Program_Clusterer );
	programs.insert( new TEPAPA_Program_CrossValidate );
	programs.insert( new TEPAPA_Program_RandomSubsampling );
	programs.insert( new TEPAPA_Program_Bootstrapping );
	programs.insert( new TEPAPA_Program_PrintTwoClassStats );
	programs.insert( new TEPAPA_Program_Ranker );
	programs.insert( new TEPAPA_Program_Print );
	programs.insert( new TEPAPA_Program_Tester );
	programs.insert( new TEPAPA_Program_Message );
	programs.insert( new TEPAPA_Program_GenerateCSV );
	programs.insert( new TEPAPA_Program_MLPRedict );
	programs.insert( new TEPAPA_Program_RegexInducer );
	programs.insert( new TEPAPA_Program_Discoverer_Regex );
	programs.insert( new TEPAPA_Program_Discoverer_BOWSentence );
	programs.insert( new TEPAPA_Program_Discoverer_BOWSentenceProfile );
	programs.insert( new TEPAPA_Program_Discoverer_SentenceProfile );
	programs.insert( new TEPAPA_Program_Pattern_Summariser );
	programs.insert( new TEPAPA_Program_Predicate_Discoverer );
	programs.insert( new TEPAPA_Program_Discoverer_MetaPattern );
	programs.insert( new TEPAPA_Program_Statistic );
	programs.insert( new TEPAPA_Program_UnlinkFile );
	programs.insert( new TEPAPA_Program_TagFile );
	programs.insert( new TEPAPA_Program_Discoverer_Gapped_Sequence);

	return true;
	}


	
void handler(int sig) {
	void *array[10];
	size_t size;

	// get void*'s for all entries on the stack
	size = backtrace(array, 10);

	// print out all the frames to stderr
	fprintf(stderr, "Error: signal %d:\n", sig);
	backtrace_symbols_fd(array, size, STDERR_FILENO);
	exit(1);
	}
	


int main(int argc, char **argv) {
	
	signal(SIGSEGV, handler);   // install our handler
		
	prog_path  = argv[0];

	TEPAPA_global_data_struct  gvr; // global variable register;
	
	initialise(gvr);
	
	command_list  commands(programs);
	
	if (argc>1) { 
		if ( strcmp(argv[1], "-") == 0 ) {
			commands.parse_from_file("/dev/stdin");
			}
		else if ( argv[1][0] == '-' ) {
			commands.parse_from_argv(argc, argv);
			}
		else if ( argv[1][0] != '@' ) {
			commands.insert( 1, argc, argv, true );
			commands.parse_from_file( argv[1] );
			}
		else {
			commands.parse_from_argv(argc, argv);
			}
		}
	else {
		msgf(VL_FATAL, "No arguments supplied.\n");
		}
	
	iptr<TEPAPA_Program>  p = commands.front().program ;
	TEPAPA_Program* pp = p;
	
	TEPAPA_Program_Main*  prog_main = dynamic_cast<TEPAPA_Program_Main*>( pp );

	assert(prog_main);
	
	prog_main -> run(gvr, commands, programs);
	
	msgf(VL_MILESTONE, "\nTotal run-time = %.3f sec\n\n", master_clock.elapsed_sec());
	
	return 0;
	}
