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

#include "tepapa-io.h"
#include "tepapa-msg.h"
#include "strfriends.h"
#include <unistd.h>

bool TEPAPA_Program_MergeDatasets::handle_argv(const vector<string>& argv) {
	
	if (argv.size() < 2 ) msgf(VL_FATAL, "Insufficient arguments specified.\nUsage: dataset1  dataset2 ... merged ");
	
	ds_name_output = argv[ argv.size() - 1 ];
	
	for(unsigned int i=0; i<argv.size() - 1; ++i) {
		ds_names_input.push_back( argv[i] );
		}
	
	return true;
	}

int TEPAPA_Program_MergeDatasets::run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) {
	TEPAPA_global_data_struct& gvr = *gvr_ptr;
	
	TEPAPA_dataset& ds_merged = gvr.get_dataset(ds_name_output);
	
	ds_merged.rr.clear();
	ds_merged.sl.clear();
	
	ds_merged = gvr.get_dataset( try_expand_variable(ds_names_input[0]) );
	
	for(unsigned int i=1; i<ds_names_input.size() ; ++i) {
		TEPAPA_dataset& ds =  gvr.get_dataset( try_expand_variable(ds_names_input[i]) );
		for(unsigned int j=0; j<ds.rr.size(); ++j) {
			ds_merged.rr.push_back( ds.rr[j] );
			}
		}
	
	return TEPAPA_RETVAL_SUCCESS | TEPAPA_RETVAL_TERMINUS;
	}

	
TEPAPA_Program_CopyDataset::TEPAPA_Program_CopyDataset()
	: TEPAPA_Program("@CopyDataset") { 
	f_do_expand = false; 
	
	options_binary.push_back( TEPAPA_option_binary("-e", "--expand-variable", "", 'b', &f_do_expand) );
	}


bool TEPAPA_Program_CopyDataset::handle_argv(const vector<string>& argv) {
	
	if (argv.size() < 2 ) msgf(VL_FATAL, "Insufficient arguments specified.");
	if (argv.size() > 2 ) msgf(VL_FATAL, "Too many arguments specified.");
	
	ds1_name = f_do_expand ? try_expand_variable(argv[0]) : argv[0] ;
	ds2_name = f_do_expand ? try_expand_variable(argv[1]) : argv[1] ;
	
	return true;
	}


int TEPAPA_Program_CopyDataset::run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) {
	TEPAPA_global_data_struct& gvr = *gvr_ptr;
	
	TEPAPA_dataset& ds1 = gvr.get_dataset( ds1_name );
	TEPAPA_dataset& ds2 = gvr.get_dataset( ds2_name );
	
	ds2 = ds1;
	
	return TEPAPA_RETVAL_SUCCESS | TEPAPA_RETVAL_TERMINUS;
	}

TEPAPA_Program_Transfer::TEPAPA_Program_Transfer(): TEPAPA_Program("@Transfer") { }

int TEPAPA_Program_Transfer::run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) {
	
	rr_output = ds_input.rr;
	
	return TEPAPA_RETVAL_SUCCESS;
	}


TEPAPA_Program_Set::TEPAPA_Program_Set(): TEPAPA_Program("@Set") { 
	f_do_expand = false; 
	options_binary.push_back( TEPAPA_option_binary("-e", "--expand-variable", "", 'b', &f_do_expand) );
	}

	
bool TEPAPA_Program_Set::handle_argv(const vector<string>& argv) {
	
	TEPAPA_global_data_struct& gvr = *gvr_ptr;
	
	if (argv.size() < 2 ) msgf(VL_FATAL, "Insufficient arguments specified.");
	if (argv.size() > 2 ) msgf(VL_FATAL, "Too many arguments specified.");
	
	gvr.variables[ argv[0] ] = f_do_expand ? gvr.variables.try_expand( argv[1] ) :  argv[1] ;
	
	return true;
	}

TEPAPA_Program_Print::TEPAPA_Program_Print(): 
	TEPAPA_Program("@Print") {
	fmt = "gdmepntbx";
	
	options_optarg.push_back( 
		TEPAPA_option_optarg("-f", "--format", "", 's', &fmt)
		);
	}


int TEPAPA_Program_Print::run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) {

	TEPAPA_Results&  rr = ds_input.rr;
	
	rr.dump(fmt);
	
	return TEPAPA_RETVAL_SUCCESS | TEPAPA_RETVAL_TERMINUS ;
	}

bool TEPAPA_Program_Message::handle_argv(const vector<string>& argv) {
	message_list = argv;
	return true;
	}

int TEPAPA_Program_Message::run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) {

	printf( "%s\n", join(" ", message_list).c_str() );
	
	return TEPAPA_RETVAL_SUCCESS | TEPAPA_RETVAL_TERMINUS ;
	}

bool TEPAPA_Program_UnlinkFile::handle_argv(const vector<string>& argv) {
	file_list = argv;
	return true;
	}

int TEPAPA_Program_UnlinkFile::run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) {

	for (unsigned int i=0; i<file_list.size(); ++i)  {
		printf( "Removing %s.\n", file_list[i].c_str() );
		int err = unlink( file_list[i].c_str() );
		if (err) perror("TEPAPA_Program_UnlinkFile::run");
		}
	
	return TEPAPA_RETVAL_SUCCESS | TEPAPA_RETVAL_TERMINUS ;
	}


int TEPAPA_Program_PrintTwoClassStats::run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) {
	
	TEPAPA_Results&  rr = ds_input.rr;
		
	rr.dump_tcs( ds_input.sl.gold_standard() );
	
	return TEPAPA_RETVAL_SUCCESS | TEPAPA_RETVAL_TERMINUS ;
	}


/////////////////////////////////////////////////////////////////////////////////
TEPAPA_Program_LoadTable::TEPAPA_Program_LoadTable(): 
	TEPAPA_Program("@LoadTable") {
		
	options_optarg.push_back( 
		TEPAPA_option_optarg("-t", "--table-name", "", 's', &csv_table_name)
		);
	}


bool TEPAPA_Program_LoadTable::handle_argv(const vector<string>& argv) {
	
// 	TEPAPA_global_data_struct& gvr = *gvr_ptr;
	if (! csv_table_name.length() ) {
// 		msgf(VL_FATAL, "No table name specified (use -t table_name) "); 
		csv_table_name = "default";
		}
	
	if (! argv.size() ) {
		msgf(VL_FATAL, "No CSV table specified");
		}
	csv_file_name = argv[0];
	
	return true;
	}


int TEPAPA_Program_LoadTable::run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) {
	
	TEPAPA_global_data_struct& gvr = *gvr_ptr;
	
	hash_value hv = ght( csv_table_name.c_str() );
	
	data_table dt ;
	dt.load( csv_file_name.c_str(), '\t' );
	dt.print();
	
	gvr.data_tables[ hv ] = dt;
	
	msgf(VL_DEBUG, "Loaded table %s -> %s (%d cols x %d rows)\n",
		csv_file_name.c_str(), ght[hv], 
		dt.cols(), dt.rows()
		);
	
	return TEPAPA_RETVAL_SUCCESS | TEPAPA_RETVAL_TERMINUS ;
	}

bool TEPAPA_Program_LoadCases::handle_argv(const vector<string>& argv) {
	
// 	TEPAPA_global_data_struct& gvr = *gvr_ptr;
	
	if (! argv.size() ) msgf(VL_FATAL, "No case definition files specified");
	
	case_definition_file = argv[0];
	
	return true;
	}


int TEPAPA_Program_LoadCases::run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) {
	
	msgf(VL_INFO, "Loading case definition file [%s]\n", case_definition_file.c_str() );

	string expanded_case_definition_file = try_expand_variable( case_definition_file.c_str() );
	
	ds_input.sl.clear();
	ds_input.sl.load( expanded_case_definition_file.c_str() );
	ds_input.sl.guess_outvar_type();
	ds_input.sl.sort();
	ds_input.sl.load_all_definitions();

	return TEPAPA_RETVAL_SUCCESS;
	}


bool TEPAPA_Program_LoadCharstring::handle_argv(const vector<string>& argv) {
	
	if (! argv.size() ) msgf(VL_FATAL, "No file describing value-string pairs specified");
	
	value_string_file = argv[0];
	
	return true;
	}


int TEPAPA_Program_LoadCharstring::run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) {
	
	msgf(VL_INFO, "Loading case value-string pair file [%s]\n", value_string_file.c_str() );

	string expanded_value_string_file = try_expand_variable( value_string_file.c_str() );
	
	ds_input.sl.clear();
	ds_input.sl.load_charstrings( expanded_value_string_file.c_str() );
	ds_input.sl.guess_outvar_type();
	ds_input.sl.sort();
	ds_input.sl.index_token_registry();

	return TEPAPA_RETVAL_SUCCESS;
	}



TEPAPA_Program_GenerateCSV::TEPAPA_Program_GenerateCSV()
	: TEPAPA_Program("@GenerateCSV") {
	delimiter = ",";
	quote = "\"";
	missing = "NA";
	f_gen_arff_headers=false;
	f_write_case_id = false;
	f_do_csv_pairs = false;
	f_numeric_attributes = false;
	
	options_binary.push_back( 
		TEPAPA_option_binary("-a", "--generate-ARFF", "", 'b', &f_gen_arff_headers, 
			[&]()->bool { 
				msgf(VL_INFO, "Generate WEKA ARFF header\n");   
				missing="?"; 
				return true; 
				} 
			) 
		);	

	options_binary.push_back( 
		TEPAPA_option_binary("-p", "--generate-CSV-pair", "", 'b', &f_do_csv_pairs) 
		);	
	
	options_binary.push_back( 
		TEPAPA_option_binary("-i", "--write-case-ID", "", 'b', &f_write_case_id) 
		);	

	options_binary.push_back( 
		TEPAPA_option_binary("-n", "--numeric-attributes", "", 'b', &f_numeric_attributes)
		);

	options_optarg.push_back( 
		TEPAPA_option_optarg("-u", "--use-dataset-as-key", "", 's', &template_dataset_name)
		);

	
	options_binary.push_back( 
		TEPAPA_option_binary("-t", "--tab-separated", "", [&]()->bool { 
			delimiter = "\t"; 
			quote = ""; 
			return true;
			} 
			)
		);	
	}



bool TEPAPA_Program_GenerateCSV::handle_argv(const vector<string>& argv) {
	
	if (f_do_csv_pairs) {
		msgf(VL_INFO, "Writing CSV pairs.\n");
		return true;
		}
	else {
		TEPAPA_global_data_struct& gvr = *gvr_ptr;
		
		if (! argv.size() ) {
			output_filename = gvr.variables[ f_gen_arff_headers ? GVAR_ACTIVE_DATASET_PREDICTIONS_ARFF : GVAR_ACTIVE_DATASET_PREDICTIONS_CSV ].c_str();
			msgf(VL_WARNING, "No output files specified. Using default output file %s\n", output_filename.c_str() );
			}
		else {
			output_filename = argv[0];
			}
		
		output_filename = gvr.variables.try_expand( output_filename );
		
		msgf(VL_INFO, "Writing to %s\n", output_filename.c_str());  
		
		return true;
		}
	}





int TEPAPA_Program_GenerateCSV::run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) {
	TEPAPA_global_data_struct& gvr = *gvr_ptr;
	
	bool f_do_csv_binary_outcome = ds_input.sl.is_outvar_binary();
	
	if ( f_do_csv_pairs ) {
		
		string ds_tr_name = gvr.variables[GVAR_ACTIVE_DATASET_TRAINING];
		string ds_ts_name = gvr.variables[GVAR_ACTIVE_DATASET_TESTING];
		
		string outf_tr = gvr.variables[ (f_gen_arff_headers ? GVAR_ACTIVE_DATASET_PREDICTIONS_TRAINING_ARFF : GVAR_ACTIVE_DATASET_PREDICTIONS_TRAINING_CSV) ];
		string outf_ts = gvr.variables[ (f_gen_arff_headers ? GVAR_ACTIVE_DATASET_PREDICTIONS_TESTING_ARFF  : GVAR_ACTIVE_DATASET_PREDICTIONS_TESTING_CSV)  ];
		
		msgf( VL_INFO, "Saving active training dataset %s to %s.\n", ds_tr_name.c_str(), outf_tr.c_str() );
		msgf( VL_INFO, "Saving active testing dataset %s to  %s.\n", ds_ts_name.c_str(), outf_ts.c_str() );
		
		if (! gvr.get_dataset(ds_tr_name).save_csv( 
				outf_tr, f_gen_arff_headers, f_do_csv_binary_outcome, f_write_case_id, f_numeric_attributes, delimiter, quote, missing
				) ) 
			return TEPAPA_RETVAL_FAILURE;
		
		if (! gvr.get_dataset(ds_ts_name).save_csv( 
				outf_ts, f_gen_arff_headers, f_do_csv_binary_outcome, f_write_case_id, f_numeric_attributes, delimiter, quote, missing
				) ) 
			return TEPAPA_RETVAL_FAILURE;
		}
	else {
		
		if ( template_dataset_name.length() ) {
			TEPAPA_dataset  ds_input_1 = ds_input;
			
			const TEPAPA_dataset&  ds_templ = gvr.get_dataset(template_dataset_name);
			
			ds_input_1.rr.reorder_results(ds_templ.rr);
			
			if (! ds_input_1.save_csv( 
				output_filename, f_gen_arff_headers, f_do_csv_binary_outcome, f_write_case_id, f_numeric_attributes, delimiter, quote, missing
				) ) return TEPAPA_RETVAL_FAILURE;
			}
		else
			{
			if (! ds_input.save_csv( 
				output_filename, f_gen_arff_headers, f_do_csv_binary_outcome, f_write_case_id, f_numeric_attributes, delimiter, quote, missing
				) ) 
				return TEPAPA_RETVAL_FAILURE;
			}
		}
	return TEPAPA_RETVAL_SUCCESS | TEPAPA_RETVAL_TERMINUS;
	}

