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

#include "tepapa-resample.h"
#include "tepapa-msg.h"
#include "strfriends.h"


TEPAPA_Program_Resample::TEPAPA_Program_Resample(const string& pname)
	: TEPAPA_Program(pname) {
	n_iters = 0 ; 
	
	options_optarg.push_back( 
		TEPAPA_option_optarg ( "-n", "--iterations", "", 'i', &n_iters )
		);

	options_optarg.push_back( 
		TEPAPA_option_optarg ( "-Otr", "--training-set-name", "", 's', &training_set_name )
		);

	options_optarg.push_back( 
		TEPAPA_option_optarg ( "-Ots", "--test-set-name", "", 's', &testing_set_name )
		);

	}



bool TEPAPA_Program_Resample::handle_argv(const vector<string>& args) {
	if (args.size() < 1 ) {
		msgf(VL_FATAL, "Usage: @Resample [options] [BEGIN|END] <label> ");
		return false;
		}
	action = args[0] ;
	
	label  = string("default");
	
	if ( args.size() > 1 ) label = args[1] ;
	
	return true;
	}


void TEPAPA_Program_Resample::define_varname() {
	varname_iters        = strfmt( "%s.%s.Iter",            getname().c_str(), label.c_str() );
	varname_current_iter = strfmt( "%s.%s.CurrentIter",     getname().c_str(), label.c_str() );
	varname_return_step  = strfmt( "%s.%s.ReturnStep",      getname().c_str(), label.c_str() );
	varname_orig_dataset = strfmt( "%s.%s.OriginalDataset", getname().c_str(), label.c_str() );
	}


int TEPAPA_Program_Resample::run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) {
	
	define_varname();
	
	TEPAPA_global_data_struct& gvr = *gvr_ptr;
  
	if ( action != string("BEGIN") && action != string("END")  )
		msgf(VL_FATAL, "Usage %s [options] [BEGIN|END] <label> ", getname().c_str() );
	
	if ( gvr.variables[ varname_orig_dataset ].length() == 0 ) {
		gvr.variables[ varname_orig_dataset ] = gvr.get_active_dataset_name();
		}
	
	if( ! ds_input.sl.size() )
		msgf(VL_FATAL, "No samples loaded!");
	
	int current_iter = gvr.variables[ varname_current_iter ];

	if ( action == "END" ) {
		n_iters = gvr.variables[ varname_iters ];
		
		if ( current_iter >= n_iters ) {
			msgf(VL_INFO, "Resampling completed.\n");
			gvr.variables[ varname_current_iter ] = 0;
			return TEPAPA_RETVAL_SUCCESS;
			}
		else {
			int return_step = gvr.variables[ varname_return_step ] ;
			gvr.variables[ GVAR_GOTO_STEP ] = return_step ;
			msgf(VL_INFO, "%s: Jump to Step %d (%s).\n", __PRETTY_FUNCTION__, return_step, gvr.variables[ GVAR_GOTO_STEP ].c_str() );
			return TEPAPA_RETVAL_SUCCESS | TEPAPA_RETVAL_JUMP;
			}
		}
		
	// action == "BEGIN"
	
	sample_list& sl = ds_input.sl;
		
	if (! current_iter ) { // new
		
		if (n_iters == 0) n_iters = 10;
		
		gvr.variables[ varname_iters ] = n_iters ;
		
		msgf( VL_INFO, "Performing %d-iteration resampling (%s)\n", n_iters, getname().c_str() );
		
		int cmd_step_to_return_to = gvr.variables[GVAR_CURRENT_STEP];
		
		gvr.variables[ varname_return_step ] = cmd_step_to_return_to ;

		msgf(VL_INFO, "Will loop back to step %d.\n", cmd_step_to_return_to);
		
		initialise(sl);
	
		msgf(VL_INFO, "Number of samples = %lu\n", sl.size() );
		}
	
	msgf(VL_INFO, "Current_iter = %d\n", current_iter);

	sample_list  sl_training ;
	sample_list  sl_testing  ;

	run_iteration( current_iter, sl_training, sl_testing );
	
	sl_training.sort();
	sl_training.index_token_registry();
	
	sl_testing.sort();
	sl_testing.index_token_registry();
		
	msgf( VL_INFO, "Iteration %d\tn(training)=%d\tn(testing)=%d\n", current_iter, sl_training.size(), sl_testing.size() );
	
	gvr.variables[ varname_current_iter ] = current_iter + 1;
	
	string  varname_dataset_training = training_set_name.length() ? 
		training_set_name : 
		strfmt( "%s.Training.Iteration.%d", getname().c_str(), current_iter );
		
	string  varname_dataset_testing  = testing_set_name.length() ? 
		testing_set_name : 
		strfmt( "%s.Testing.Iteration.%d",  getname().c_str(), current_iter );

	gvr.get_dataset( varname_dataset_training ).sl = sl_training;
	gvr.get_dataset( varname_dataset_testing  ).sl = sl_testing;
	
	gvr.variables[ GVAR_ACTIVE_DATASET_TRAINING ] = varname_dataset_training ;
	gvr.variables[ GVAR_ACTIVE_DATASET_TESTING  ] = varname_dataset_testing  ;

	string varname_stem = strfmt( "iter-%d", current_iter );
	
	gvr.variables[ GVAR_ACTIVE_DATASET_PREDICTIONS_STEM ] = varname_stem ;
	
	string  varname_training_csv_filename = strfmt( "%s-tr.csv", varname_stem.c_str() );
	string  varname_testing_csv_filename  = strfmt( "%s-ts.csv", varname_stem.c_str() );
	
	gvr.variables[ GVAR_ACTIVE_DATASET_PREDICTIONS_TRAINING_CSV ] = varname_training_csv_filename ;
	gvr.variables[ GVAR_ACTIVE_DATASET_PREDICTIONS_TESTING_CSV  ] = varname_testing_csv_filename  ;

	string  varname_training_arff_filename = strfmt( "%s-tr.arff", varname_stem.c_str() );
	string  varname_testing_arff_filename  = strfmt( "%s-ts.arff", varname_stem.c_str() );
	
	gvr.variables[ GVAR_ACTIVE_DATASET_PREDICTIONS_TRAINING_ARFF ] = varname_training_arff_filename ;
	gvr.variables[ GVAR_ACTIVE_DATASET_PREDICTIONS_TESTING_ARFF  ] = varname_testing_arff_filename  ;

	return TEPAPA_RETVAL_SUCCESS | TEPAPA_RETVAL_TERMINUS;
	}


////////////////////////////////////////////////////////////////////////////////////////////////////////

unsigned int TEPAPA_Program_CrossValidate::initialise(sample_list&  sl_original) {
	sl_original.sort(); msgf(VL_INFO, "Samples stratified\n");
	
	sl_original.shuffle();  msgf(VL_INFO, "Samples shuffled\n");
	
	return sl_original.size();
	}
	
unsigned int TEPAPA_Program_CrossValidate::run_iteration(int current_iter, sample_list&  sl_training, sample_list& sl_testing ) {
	TEPAPA_global_data_struct& gvr = *gvr_ptr;
	
	TEPAPA_dataset& active_dataset = gvr.get_dataset( gvr.variables[ varname_orig_dataset ] );
	
	sample_list& sl = active_dataset.sl;
	
	for(unsigned int i=0; i<sl.size(); ++i) {
		int group = i % get_n_iters() ;

		if (group == current_iter)
			sl_testing. push_back( sl[i] );
		else 
			sl_training. push_back( sl[i] );
		}
	return sl_training.size() + sl_training.size();
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////
	
unsigned int TEPAPA_Program_Bootstrapping::run_iteration(int current_iter, sample_list&  sl_training, sample_list& sl_testing ) {
	TEPAPA_global_data_struct& gvr = *gvr_ptr;
	
	TEPAPA_dataset& active_dataset = gvr.get_dataset( gvr.variables[ varname_orig_dataset ] );
	
	sample_list& sl = active_dataset.sl;
	
	sl.shuffle();
	
	vector<unsigned int> sl_b_i( sl.size() );
	vector<bool> sl_b( sl.size() );
	
	for(unsigned int i=0; i<sl.size(); ++i) sl_b_i[i] = random() % sl.size();
	for(unsigned int i=0; i<sl_b_i.size(); ++i) sl_b[i] = false;
	for(unsigned int i=0; i<sl_b_i.size(); ++i) sl_b[ sl_b_i[i] ] = true;
	
	for(unsigned int i=0; i<sl_b_i.size(); ++i) {
		sl_training.push_back( sl[ sl_b_i[i] ] );
		}
		
	for(unsigned int i=0; i<sl.size(); ++i) {
		if (! sl_b[i]) sl_testing.push_back( sl[i] );
		}
	return sl_training.size() + sl_training.size();
	}


////////////////////////////////////////////////////////////////////////////////////////////////////////

TEPAPA_Program_RandomSubsampling::TEPAPA_Program_RandomSubsampling()
	: TEPAPA_Program_Resample("@RandomSubsample") { 
	r_training = 0.5;
	
	options_optarg.push_back( 
		TEPAPA_option_optarg (
			"-p", "--percent-training", "", 
			[&](const string& optarg) -> bool { 
				r_training = atof( optarg.c_str() );
				return true;
				}
			)
		); 
	}


unsigned int TEPAPA_Program_RandomSubsampling::run_iteration(int current_iter, sample_list&  sl_training, sample_list& sl_testing ) {
	TEPAPA_global_data_struct& gvr = *gvr_ptr;
	
	TEPAPA_dataset& active_dataset = gvr.get_dataset( gvr.variables[ varname_orig_dataset ] );
	
	sample_list& sl = active_dataset.sl;
	
	sl.shuffle();
	
	for(unsigned int i=0; i<sl.size(); ++i) {
		bool testing_group = ( drand48() > r_training );
		
		if (testing_group)
			sl_testing.push_back( sl[i] );
		else 
			sl_training.push_back( sl[i] );
		}

	return sl_training.size() + sl_training.size();
	}
