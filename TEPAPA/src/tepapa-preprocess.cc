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

#include "tepapa-preprocess.h"
#include "tepapa-msg.h"
#include "tepapa-utils.h"
#include "utils.h"
#include "strfriends.h"
#include "eperl.h"

#include <string>

using namespace std;
#include <vector>
#include <sys/stat.h>
#include <sys/types.h>

#include "pipe.h"

// #define TEPAPA_MULTITHREAD  0

// #undef TEPAPA_MULTITHREAD 

#if TEPAPA_MULTITHREAD 
#include <future>
#include <functional>
#include <unistd.h>
#include "parallel.h"
#endif // TEPAPA_MULTITHREAD



void process_sample(hash_value& out_hv, const sample_list& sl_in, const vector<preprocess_action>& actions, unsigned int i) ;

void process_sample(hash_value& out_hv, const sample_list& sl_in, const vector<preprocess_action>& actions, unsigned int i) {
	hash_value hv = sl_in[i].definitions[0];
	string start_path = ght[ hv ];
	string start_dirname  = dirname_r(start_path);
	string start_basename = basename_r(start_path);
	if (! file_exists(start_path) ) {
		msgf( VL_FATAL, "Unable to load source file / corpus %s", start_path.c_str() );
		}
		
// 	msgf( VL_DEBUG, "Source file (#%d): %s\n", i+1, start_path.c_str());
	
	string prev_dirname = start_dirname;
	string prev_path = start_path;
	
	for(unsigned int j=0; j<actions.size(); ++j){
		string final_dirname = start_dirname + string("/") + actions[j].outdir;
		string final_path    = final_dirname + string("/") + start_basename;
		
		string msg = strfmt("# Source %d (%s) [%d:%-s] %s < %s > %s ", 
			i+1, start_basename.c_str(), 
			j+1, actions[j].action.c_str(), 
			actions[j].command.c_str(),
			prev_path.c_str(),
			final_path.c_str(),
			actions[j].args.c_str()
			);
			if ( ( ! dir_exists(final_dirname) ) ) {
			if ( mkdir( final_dirname.c_str() , 0755) != 0 ) {
				msgf( VL_FATAL, "Failed to create dir %s", final_dirname.c_str() );
				}
			}
		
		if ( ( ! file_exists(final_path) ) || 
			( mtime(final_path) < mtime(prev_path) ) ||
			( mtime(final_path) < mtime(actions[j].command) )
			) {
			string data = load_file(prev_path);
			msgf( VL_INFO, "%s (run)\n", msg.c_str());
			string retval = pipe_command (actions[j].command, data) ;
// 			fprintf(stderr,"%s", retval.c_str());
			save_file(final_path, retval);
			}
		else {
			msgf( VL_INFO, "%s (cached)\n", msg.c_str());
			}
		
		prev_dirname = final_dirname;
		prev_path = final_path;
		}
	
	out_hv = ght( prev_path.c_str() ) ;
// 	hash_value hv_filename_final_path 
// 		
// 	return hv_filename_final_path ;
	}

TEPAPA_Program_Preprocess::TEPAPA_Program_Preprocess()
	: TEPAPA_Program("@Preprocess") {
		
	options_optarg.push_back( 
		TEPAPA_option_optarg ("-i", "--input-file", "", 's', &path_case_raw)
		); 
	
	options_optarg.push_back( 
		TEPAPA_option_optarg ("-o", "--output-file", "", 's', &path_case_processed)
		); 
	}


	
bool TEPAPA_Program_Preprocess::handle_argv(const vector<string>& argv) {
	msgf(VL_INFO, "prog_path=%s\n", prog_path.c_str());
	
	EmbeddedPerl  eperl(true);

	string prog_dir = eperl.simple_replacement(prog_path, "(.*)/.*", "$1");
	
	msgf(VL_INFO, "prog_dir=%s\n", prog_dir.c_str());

	for(unsigned int i=0; i<argv.size(); ++i) {
		string s = argv[i];
		string s_lc = strtolower(s);
		string s_uc = strtoupper(s);
		string sname = eperl.simple_replacement(s_lc, "[[:punct:]]+", "");
		string sdname = string("post-") + sname ;
		
		string cmdfn = find_executable(
			{ s, s_lc, s_uc },                                        // program name variations
			{ "./", prog_dir + "/", prog_dir + "/../scripts/", "" },  // directories
			{ "prep-", "" },                                          // program name prefixes
			{ ".pl", ".sh", ".py", "" }                               // program name suffixes
			);
		
		if ( ! cmdfn.size() ) {
			msgf(VL_FATAL, "i=%d\targv[i]=%s\tNo matching command", i, argv[i].c_str() );
			}
		msgf(VL_INFO, "i=%d\targv[i]=%s\tcmd=%s\tsname=%s\n", i, argv[i].c_str(), cmdfn.c_str(), sname.c_str());
		
		preprocess_action  pa;
		pa.action  = s;
		if ( actions.size() ) {
			pa.outdir += actions[actions.size() - 1].outdir ;
			pa.outdir += "/"; 
			}
		pa.outdir += sdname ;

		pa.command = cmdfn;
		pa.args    = "";
		actions.push_back(pa);
		};
	
	return true;
	}


int TEPAPA_Program_Preprocess::run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) {
	// SYNTAX: @Preprocess [-o output case] case [CLEAN|ANNOTATE] TOKENISE ...
	
	EmbeddedPerl  eperl(true);
	
	if ( ! path_case_raw.length() )  {
		msgf(VL_FATAL, "No case definition source file specified.");
		}
	
	if ( ! path_case_processed.length() )  {
		path_case_processed = eperl.simple_replacement("(\\.txt)$", path_case_raw, "-preped$1");
		if (path_case_processed == path_case_raw) {
			msgf(VL_FATAL, "Output filenames are identical. Please specify -i and -o options.");
			}
		}
	
	msgf( VL_INFO, "Input  = %s\n", path_case_raw.c_str() );
	msgf( VL_INFO, "Output = %s\n", path_case_processed.c_str() );
	for(unsigned int i=0; i<actions.size(); ++i){
		msgf( VL_INFO, "%s\t%s\t%s\t%s\n", 
			actions[i].action.c_str(),  actions[i].outdir.c_str(), 
			actions[i].command.c_str(), actions[i].args.c_str()
			);
		}

	
	sample_list  sl_in ;
	
	if (! sl_in.load( path_case_raw.c_str() ) ) {
		msgf( VL_FATAL, "Unable to load case file %s", path_case_raw.c_str() );
		}


	hash_value  sl_out_hv_filename_final_paths[ sl_in.size() ];

#if TEPAPA_MULTITHREAD
	thread_manager tm;
#endif // TEPAPA_MULTITHREAD

	
	for(unsigned int i=0; i<sl_in.size(); ++i) {
#if TEPAPA_MULTITHREAD
		tm.queue();
		std::async( std::launch::async, &process_sample, std::ref(sl_out_hv_filename_final_paths[i]),  std::ref(sl_in),  std::ref(actions), i);
		
// 		sl_out_hv_filename_final_paths[i] = std::async<hash_value>(process_sample, std::ref(sl_in), std::ref(actions), i);
		;
#else 
		process_sample(sl_out_hv_filename_final_paths[i], sl_in, actions, i);
#endif // TEPAPA_MULTITHREAD
		}

#if TEPAPA_MULTITHREAD
	tm.join_all();
#endif // TEPAPA_MULTITHREAD
	

	sample_list  sl_out = sl_in ;
	
	for(unsigned int i=0; i<sl_in.size(); ++i) {
		hash_value hv_filename_final_path  = sl_out_hv_filename_final_paths[i];
		
		if (sl_out[i].definitions.size() == 0 ) {
			sl_out[i].definitions.push_back (hv_filename_final_path );
			}
		else {
			sl_out[i].definitions[0] = hv_filename_final_path;
			}
		}

	sl_out.save(path_case_processed);
	
	return TEPAPA_RETVAL_SUCCESS | TEPAPA_RETVAL_TERMINUS;
	}
