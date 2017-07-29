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

#include "tepapa-loop.h"
#include "tepapa-msg.h"
#include "strfriends.h"


TEPAPA_Program_GenerateCases::TEPAPA_Program_GenerateCases()
	: TEPAPA_Program("@GenerateCases") {
	
	options_optarg.push_back( 
		TEPAPA_option_optarg ( "-t", "--table-name", "", 's', &table_name )
		);
	}


void TEPAPA_Program_GenerateCases::define_varname() {
	varname_varlist      = strfmt( "%s.%s.VarList",                    getname().c_str(), label.c_str() );
    varname_varlist_inited = strfmt( "%s.%s.VarList.Initialised",      getname().c_str(), label.c_str() );
	varname_return_step  = strfmt( "%s.%s.ReturnStep",                 getname().c_str(), label.c_str() );
	varname_table_name   = strfmt( "%s.%s.TableName",                  getname().c_str(), label.c_str() );
	}


string TEPAPA_Program_GenerateCases::generate_tabbed_string(const vector<string>& v) {
	return join("\t", v);
	}

bool TEPAPA_Program_GenerateCases::handle_argv(const vector<string>& args) {
	if (args.size() < 1 ) {
		msgf(VL_FATAL, "Usage: @ForAllVariables [options] [BEGIN|END] [var1], [var2], [...] ");
		return false;
		}
	action = args[0] ;
	
	label = string("default");
	
	define_varname();
	
	TEPAPA_global_data_struct& gvr = *gvr_ptr;
	
	if ( action == "BEGIN" ) {
		if ( int(gvr.variables[ varname_varlist_inited ]) == 0 ) {
			gvr.variables[ varname_varlist ] = generate_tabbed_string( vector<string>(args.begin() + 1, args.end()) );
		
			int cmd_step_to_return_to = gvr.variables[GVAR_CURRENT_STEP];
			
			gvr.variables[ varname_return_step ] = cmd_step_to_return_to ;
	
			msgf(VL_INFO, "Will loop back to step %d.\n", cmd_step_to_return_to);
			
			gvr.variables[ varname_varlist_inited ] = 1;
			}
		}
	
	return true;
	}


int TEPAPA_Program_GenerateCases::run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) {
	
	TEPAPA_global_data_struct& gvr = *gvr_ptr;
  
	if ( action != string("BEGIN") && action != string("END")  )
		msgf(VL_FATAL, "Usage %s [options] [BEGIN|END] <label> ", getname().c_str() );
	
	if ( gvr.variables[ varname_table_name ].length() == 0 ) {
		gvr.variables[ varname_table_name ] = "default";
		}
	
	if ( action == "END" ) {
		string s = gvr.variables[ varname_varlist ];
// 		printf("%s=%s\n", varname_varlist.c_str(), s.c_str());
		if ( s.length() == 0 ) {
			msgf(VL_INFO, "Loop completed.\n");
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
	
	if ( ! gvr.variables[ varname_varlist ].length() ) { // no specified, thus use EVERYTHING
		hash_value hv = ght( table_name.c_str() );
	
		const data_table& dt = gvr.data_tables[ hv ];
		vector<string> fields = dt.get_fields();

		for (unsigned 	int i=0; i<fields.size(); ++i) {
			printf("%s\n", fields[i].c_str() );
			printf("\t%s\n", join("\t", dt.get_levels(fields[i])).c_str() );
			}
		}
	
	string s = gvr.variables[ varname_varlist ] ;
	
	size_t e = s.find('\t') ;
	
	string v;
	
	if (e==string::npos) {
		v = s;
		gvr.variables[ varname_varlist ] = "";
		}
	else {
		v = s.substr(0, e);
		gvr.variables[ varname_varlist ] = s.substr(e+1);
		}
	
	printf("%s\n", v.c_str());
	
	return TEPAPA_RETVAL_SUCCESS | TEPAPA_RETVAL_TERMINUS;
	}
