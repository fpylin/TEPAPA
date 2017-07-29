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

#include "tepapa-program.h"
#include "tepapa-msg.h"
#include "strfriends.h"
#include <typeinfo>

void TEPAPA_option_optarg::set_default_arghelp() {
	switch(vartype) {
		case 's': arghelp = "string"; break;
		case 'f': arghelp = "number"; break;
		case 'i': arghelp = "integer"; break;
		break;
		};
	}


bool TEPAPA_option_binary_default_func_proper() {return true;}
bool TEPAPA_option_optarg_default_func_proper(const std::string&) {return true;}

std::function<bool()>                    TEPAPA_option_binary::default_func = TEPAPA_option_binary_default_func_proper;
std::function<bool(const std::string&)>  TEPAPA_option_optarg::default_func = TEPAPA_option_optarg_default_func_proper;


TEPAPA_Program::TEPAPA_Program(const string& pname): __iptr_member () { 
	gvr_ptr = 0; 
	name = pname;
	
	f_export_append = false;
	f_pipe_results  = true;
	
	options_binary.push_back(
		TEPAPA_option_binary("-H", "--help", "Help page", [&]()->bool{
			string help_msg = get_help_message();
			printf("%s\n", help_msg.c_str());
			exit(1);
			return false;
			}
			)
		);

	options_optarg.push_back( 
		TEPAPA_option_optarg ( "-D", "--discovery-set", "The discovery, input, or training dataset", 's', &active_dataset_name)
		); 
	
	options_optarg.push_back( 
		TEPAPA_option_optarg ( "-T", "--test-set", "The test dataset", 's', &test_dataset_name)
		);

	options_optarg.push_back( 
		TEPAPA_option_optarg ( "-O", "--output", "The output dataset", 's', &export_dataset_name,
			[&](const string& optarg) -> bool { 
				f_pipe_results = false;
				return true;
				}
			)
		);

	options_binary.push_back(
		TEPAPA_option_binary("-A", "--append", "Appending the results to the dataset specified by -O", 'b', &f_export_append)
		);
	}


void TEPAPA_Program::set_default_datasets_name() {
	assert(gvr_ptr);
	
	TEPAPA_global_data_struct&  gvr = * gvr_ptr;
	
	active_dataset_name = gvr.variables[GVAR_ACTIVE_DATASET_TRAINING];  // active discovery data set
	
	if ( ! active_dataset_name.length() )  active_dataset_name = gvr.get_active_dataset_name(); 
	
	assert( active_dataset_name.length() );

	test_dataset_name = gvr.variables[GVAR_ACTIVE_DATASET_TESTING]; // test discovery data set
	
	if ( ! test_dataset_name .length() )  test_dataset_name = gvr.get_active_dataset_name(); 
	
	assert( test_dataset_name.length() );
	}

// vector<string> TEPAPA_Program::argv_to_vector(int argc, char** argv) {
// 	vector<string>  retval;
// 	for(int i=0; i<argc; ++i)  retval.push_back(argv[i]);
// 	return retval;
// 	}


bool TEPAPA_Program::handle_options_binary(const string& opt) { 
// 	printf( "%s\t[%s]\n", __PRETTY_FUNCTION__, opt.c_str() );
	for(unsigned int i=0;i<options_binary.size(); ++i) {
		const TEPAPA_option_binary& opt_i = options_binary[i];
// 		printf( "%d\t%s\n", i, opt_i.name.c_str() );
		if ( opt_i.name != opt && opt_i.lname != opt ) continue;
		
		param[opt_i.name] = 0;
		
		switch( opt_i.vartype ) {
			case 0:   break; // no attached variables to set;
			case 'b': 
				if (opt_i.varptr) {
					(*(bool*)opt_i.varptr) = true; 
					}
				param[opt_i.name] = 1;
				break;
			default: 
				msgf(VL_FATAL, "Invalid data type `%c'", opt_i.vartype );
			};
		
		msgf(VL_DEBUG, "%s: Setting binary option `%s'\n", getname().c_str(), opt.c_str());

		if ( ! opt_i.func() ) {
			msgf(VL_FATAL, "Unable to set option `%s'", opt.c_str());
			}
		return true; // handled
		}
	return false; // not handled 
	}


bool TEPAPA_Program::handle_options_optarg(const string& opt, const string& optarg) { 
// 	printf( "%s\t%s\n", __PRETTY_FUNCTION__, opt.c_str() );
	for(unsigned int i=0;i<options_optarg.size(); ++i) {
		const TEPAPA_option_optarg& opt_i = options_optarg[i];
// 		printf( "%d\t%s\n", i, opt_i.name.c_str() );
		if ( opt_i.name != opt && opt_i.lname != opt ) continue;
		
		switch( opt_i.vartype ) {
			case 0:   break; // no attached variables to set;
			case 's': {
				string s = try_expand_variable( optarg );
				if (opt_i.varptr) {
					(*(std::string*)opt_i.varptr) = s; 
					}
				param[opt_i.name] = s;
				break; 
				}
				
			case 'f': {
				string s = try_expand_variable( optarg );
				double vd = atof( s.c_str() );
				if (opt_i.varptr) {
					(*(double*)opt_i.varptr) = vd;
					}
				param[opt_i.name] = vd;
				break; 
				}
				
			case 'i': {
				string s = try_expand_variable( optarg );
				int vi = atof( s.c_str() );
				if (opt_i.varptr) {
					(*(int*)opt_i.varptr) = vi;
					}
				param[opt_i.name] = vi;
				break;
				}
				
			default: 
				msgf(VL_FATAL, "Invalid data type `%c'", opt_i.vartype );
			};
		
		msgf(VL_DEBUG, "%s: Setting option `%s' with value `%s'\n", getname().c_str(), opt.c_str(), optarg.c_str() );
		
		if ( ! opt_i.func(optarg) ) {
			msgf(VL_FATAL, "Unable to set option `%s'", opt.c_str());
			}
		
		return true; // handled
		}
	
	return false; // not handled 
	}


int TEPAPA_Program::args_find_next_option(const vector<string>& args, int start) {
	for (unsigned int i=start; i<args.size(); ++i) {
		if ( (args[i].length() > 1) && (args[i][0] == '-') ) {
			return i;
			}
		}
	return args.size();
	}


bool TEPAPA_Program::process_command_line(const vector<string>& args) {
	
	vector<string> remaining_args;
	
	assert(args.size() > 0);
	
	string argv0 = args[0];
	
	unsigned int i=1, j;
	
	do  {
		j = args_find_next_option(args, i);
		
// 		msgf(VL_INFO, "i=%d\tj=%d\n", i, j);
		
		for (; i<j; ++i) remaining_args.push_back( args[i] );
		
		if ( i==args.size() ) break;
		
		if ( handle_options_binary(args[i]) ) { ++i;  continue; }
		
		j = args_find_next_option(args, i+1);
		
		vector<string> sub_args = vector<string>(args.begin() + i + 1, args.begin() + j);
		
// 		fprintf(stderr, "\n!i+1=%d\tj=%d\t%d\n",i+1,j,sub_args.size());
		
		if ( sub_args.size() == 0) msgf(VL_FATAL, "Option `%s' requires an argument", args[i].c_str() );
	
		const string& optarg = sub_args[0];
				
		if ( handle_options_optarg(args[i], optarg) ) { i += 2; continue; }
			
		msgf(VL_FATAL, "Unknown option `%s'.\n", args[i].c_str());
		}
	while (i<args.size()) ;
	
	vector<string>  expanded_remaining_args ;
	
	for(unsigned int i=0; i<remaining_args.size(); ++i) {
		string s = remaining_args[i] ;
		string e = try_expand_variable( s ) ;
		expanded_remaining_args.push_back( e );
// 		printf("!\t%s\t%s\n", s.c_str(), e.c_str());
		}
	
	if (! handle_argv( expanded_remaining_args ) ) return false;

	return true;
	}


TEPAPA_dataset&  TEPAPA_Program::get_active_dataset () {
	assert(gvr_ptr);
	
	TEPAPA_global_data_struct&  gvr = * gvr_ptr;
	
	string dereferenced_name = gvr.variables.try_expand(active_dataset_name);
	
	return gvr.get_dataset( dereferenced_name );
	}

TEPAPA_dataset&  TEPAPA_Program::get_export_dataset () {
	assert(gvr_ptr);
	
	TEPAPA_global_data_struct&  gvr = * gvr_ptr;
	
	string dereferenced_name = gvr.variables.try_expand(export_dataset_name);
	
	return gvr.get_dataset( dereferenced_name );
	}

TEPAPA_dataset&  TEPAPA_Program::get_test_dataset () {
	assert(gvr_ptr);
	
	TEPAPA_global_data_struct&  gvr = * gvr_ptr;
	
	string dereferenced_name = gvr.variables.try_expand(test_dataset_name);
	
	return gvr.get_dataset( dereferenced_name );
	}

// virtual int run(TEPAPA_dataset&  ds_input, TEPAPA_dataset&  ds_output) ;

int TEPAPA_Program::total_runs =0 ;

int TEPAPA_Program::run(TEPAPA_global_data_struct& gvr, const vector<string>& args) {
// 	msgf(VL_INFO, "Class %s\n", typeid( *this ).name());

	set_global_data(gvr) ;
		
	set_default_datasets_name();
	
	if (! process_command_line(args) ) return TEPAPA_RETVAL_FAILURE;
	
	initialise();
	
	msgf(((step_no==0)?VL_DEBUG:VL_MILESTONE), "=== STEP %03d (%d): %s ===\n", step_no, total_runs, join(" ", args).c_str() );

	++total_runs;
	
	TEPAPA_dataset& ds_input = get_active_dataset();

	TEPAPA_Results rr_output;
	
	msgf(VL_INFO, "Using dataset [%s]\n", active_dataset_name.c_str());
	
	int retval = run(ds_input, rr_output);
	
	if ( ( retval & TEPAPA_RETVAL_SUCCESS_MASK ) != TEPAPA_RETVAL_SUCCESS)  return retval ; 
		
	if ( ( retval & TEPAPA_RETVAL_TERMINUS ) ) return retval;
	
	if ( f_pipe_results ) { // write back to the original dataset 
		if (f_export_append) {
			ds_input.rr.append( rr_output );
			}
		else {
			ds_input.rr = rr_output;
			}
		}
	else { // write to a separate dataset 
		if ( export_dataset_name.size() ) {
			TEPAPA_dataset&  export_dataset = get_export_dataset();
			
			if (f_export_append) {
				export_dataset.rr.append( rr_output );
				}
			else {
				export_dataset.rr = rr_output;
				}
			
			export_dataset.sl = ds_input.sl;
			}
		}
	
	return retval;
	}



string TEPAPA_Program::get_help_message() const {
	string s;
	s += strfmt("\nProgram [%s]\nHelp:\n", name.c_str()) ;
	
	s += "Usage: " + name + "\n" ;
	
	s += string("\nBinary switches:\n") ;
	for(unsigned int i=0; i<options_binary.size(); ++i) {
		s   += "\t" 
			+ strfmt("%-4s %-8s %-16s ", options_binary[i].name.c_str(), "", options_binary[i].lname.c_str())
			+ options_binary[i].help 
			+ "\n";
		}
	
	s += string("\nOptions:\n") ;
	for(unsigned int i=0; i<options_optarg.size(); ++i) {
		s   += "\t" 
			+ strfmt("%-4s %-8s %-16s ", options_optarg[i].name.c_str(), options_optarg[i].arghelp.c_str(), options_optarg[i].lname.c_str())
			+ options_optarg[i].help.c_str()
			+ "\n";
		}
	
	return s;
	}
