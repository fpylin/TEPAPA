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
#include "tepapa-main.h"
#include "strfriends.h"

#include "tepapa-msg.h"

void TEPAPA_Program_Main::title(void) const {
	msgf(VL_MILESTONE, "TEPAPA - a Text mining-based Exploratory Pattern Analyser for Prognostic and Associative factor discovery\n");
// 	msgf(VL_MILESTONE, "By Frank P.Y. Lin , (C) 2015-2016, version %s (Compiled %s %s)\n", VERSION_STRING, __DATE__, __TIME__);
	msgf(VL_MILESTONE, "Version %s, compiled %s %s\n\n", VERSION_STRING, __DATE__, __TIME__);
	}


TEPAPA_Program_Main::TEPAPA_Program_Main(): 
	TEPAPA_Program("@Main") {
		
	f_do_help = false;
	f_quiet = false;
	verbosity_level = VL_MILESTONE;
	
	options_binary.push_back(
		TEPAPA_option_binary("-q", "--quiet", "", 'b', &f_quiet, [&] () -> bool {
			msg_verbosity_level = 0 ;
			return true;
			}
			) 
		);
	
	options_optarg.push_back(
		TEPAPA_option_optarg("-v", "--verbosity", "", 'i', &verbosity_level, [&] (const string& optarg) -> bool {
			msg_verbosity_level = verbosity_level ;
			return true;
			}
			) 
		);
	
	options_binary.push_back(
		TEPAPA_option_binary("-h", "--help", "", 'b', &f_do_help)
		);

	}

void TEPAPA_Program_Main::initialise() {
	}

int TEPAPA_Program_Main::run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) { 

	if (f_do_help) {
		const program_list&  programs = *programs_ptr;
		
		for(auto it = programs.begin(); it != programs.end(); ++it) {
			string  s = it -> second -> get_help_message();
			printf( "%s\n", s.c_str() );
			}
		}
	
	const command_list& commands = *commands_ptr;
	
	title();
	
	msgf(VL_DEBUG, "%d-step pipeline created at %s\n", commands.size(), master_clock.get_now_str().c_str());
	for(unsigned int i=0; i<commands.size(); ++i) {
		msgf(VL_DEBUG, "Step %3d:\t%s\n", i+1, commands[i].to_string(" ").c_str() );
		}

	return TEPAPA_RETVAL_SUCCESS | TEPAPA_RETVAL_TERMINUS ;
	}


bool TEPAPA_Program_Main::handle_argv(const vector<string>& argv) {
	TEPAPA_global_data_struct& gvr = *gvr_ptr;

	for(unsigned int i=0; i<argv.size() ; ++i) {
		string varname = strfmt("ARGV%d", i+1);
		gvr.variables[ varname ] =  argv[i];
		}    
	return true;
	}

int TEPAPA_Program_Main::run(TEPAPA_global_data_struct& gvr, command_list& commands, const program_list&  programs) {
	
	programs_ptr = &programs;
	commands_ptr = &commands;
	
	unsigned int step = 0;
	unsigned int step_cnt = 0;
	
	while ( step < commands.size() ) {
		
		++step_cnt;
		
		stopwatch  sw;
		
		sw.start();
		
		gvr.variables[ GVAR_CURRENT_STEP ] = step;
		
		sub_program = commands[step].program->getname();
		
		commands[step].program->set_step_no(step);
		
		int retval = commands[step].program->run(gvr, commands[step].args);
		
		int retval_success = (retval & TEPAPA_RETVAL_SUCCESS_MASK);
		
		if ( retval_success == TEPAPA_RETVAL_SUCCESS ) {
			msgf(VL_INFO, "Step %03d completed. (Elapsed = %.3f sec, Return value = %d)\n", step, sw.elapsed_sec(), retval);
			}
		else { // if ( retval_success == TEPAPA_RETVAL_FAILURE) {
			msgf(VL_ERROR, "Step %03d failed! (Elapsed = %.3f sec, Return value = %d)\n", step, sw.elapsed_sec(), retval);
			}
				
		if ( (retval & TEPAPA_RETVAL_JUMP) ) {
			int step_jump = gvr.variables[ GVAR_GOTO_STEP ];
			msgf(VL_DEBUG, "%s: Jump to step %d", __PRETTY_FUNCTION__, step_jump);
			step = step_jump;
			}
		else {
			++step;
			}
		
		msgf(VL_INFO, "Datasets:\n");
		gvr.dump_datasets();
		
		msgf(VL_INFO, "Variables:\n");
		gvr.variables.debug_dump();
		
		fflush(stdout);
		}
	return TEPAPA_RETVAL_SUCCESS | TEPAPA_RETVAL_TERMINUS;
	}
