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
#include "tepapa-command.h"
#include "tepapa-msg.h"
#include "strfriends.h"

iptr<TEPAPA_Program> program_list::find_program(const string& str) const {
	iptr<TEPAPA_Program> null;
	const_iterator z = find(str); 
	return ( z != end() ) ? z->second : null;
	}

void program_list::insert(TEPAPA_Program* pp) {
	string key = pp->getname();
	iptr<TEPAPA_Program> p = pp; 
	map<string, iptr<TEPAPA_Program> >::insert( pair< string,iptr<TEPAPA_Program> >(key, p) );
	}

void command_structure ::set_primary_program(const program_list&  pl) {
		
	iptr<TEPAPA_Program> p = pl.find_program( args[0] );
	
	if ( !p ) msgf(VL_FATAL, "Invalid command %s", args[0].c_str() );
	
	program = p ->spawn();
	}

	
void command_structure ::set(const program_list&  pl, int p_argi_begin, int p_argi_end, char** p_argv, bool is_tepapa_main) {
	int i;
	args.clear();
	for(i=0; i<1024; ++i) {
		if ( (p_argi_begin + i) >= p_argi_end) break;
		args.push_back( p_argv[p_argi_begin+i] );
		}
	
	if (is_tepapa_main) {
		if ( args.size() ) 
			args[0] = COMMAND_NAME_MAIN ;
		else 
			args.push_back( COMMAND_NAME_MAIN ) ;
		}
	
	set_primary_program(pl);
	}

void command_structure::set(const program_list&  pl, const vector<string>& strs, bool is_tepapa_main) {
	unsigned int i;
// 	printf("%d\t%d\n", p_argi_begin, p_argi_end);
	args.clear();
	for(i=0; i<strs.size(); ++i) args.push_back( strs[i] );
	
	if (is_tepapa_main) {
		if ( args.size() ) 
			args[0] = COMMAND_NAME_MAIN ;
		else 
			args.push_back( COMMAND_NAME_MAIN ) ;
		}
	
	set_primary_program(pl);
	}


string command_structure::to_string(const char* delim) const {
	return join(delim, args);
	}


int command_list::find_next_command(int i, int argc, char **argv) {
	while(i<argc) {
		if ( argv[i][0] == '@' ) return i;
		++i;
		}
	return argc;
	}
	
command_list::command_list()
	: vector<command_structure> (){
	}

command_list::command_list(const program_list& pl_ref)
	: vector<command_structure> (){
	set_program_list(pl_ref);
	}
	
void command_list::insert(const vector<string>& argv) {
	const program_list& pl_ref = *pl_ptr;
	command_structure  cs(pl_ref, argv);
	push_back( cs );
	}

void command_list::insert(int p_argi_begin, int p_argi_end, char** p_argv, bool is_tepapa_main) {
	const program_list& pl_ref = *pl_ptr;
	command_structure  cs(pl_ref, p_argi_begin, p_argi_end, p_argv, is_tepapa_main);
	push_back( cs );
	};

#include "parse.h"
	

int command_list::parse_from_file(const string& filename) {
	string s ;
	FILE* fin = fopen(filename.c_str(), "rt") ;
	if (! fin) msgf(VL_FATAL, "Unable to open script %s.\n", filename.c_str() ); 
	
	msgf(VL_DEBUG, "Loading commands from %s.\n", filename.c_str());
	
	char buf[65536];
	
	vector<string> tokens;
	while( fgets(buf, 65535, fin) ) {
		remove_comments(buf);
		s += buf;
		};
	fclose(fin);
	
	char* str = strdupa( s.c_str() );
	char* z = str;
	
	vector<string> argv;
	
// 	printf("!%s");
// 	commands.clear();
	do  {
		z = parse_skip_whitespaces(z) ;
		
		switch( *z ) {
			case 0: break;
			case '\'':
			case '"':
				if ( ! (z = parse_quoted_string(z, buf, *z) ) );
				break;
			default:
				z = parse_capture_until_chars(z, " \t\n\r", buf);
				if ( buf[0] == '@' ) {
					if ( argv.size() > 0 ) {
						insert(argv);
						argv.clear();
						}
					else {
						}
// 					if ( commands.size() > 0 ) printf("->\n");
					}
				break;
			};
		if (!*z) break;
// 		printf("[%s] ", buf);
		argv.push_back(buf);
		}
	while( z && *z );
	
	if ( argv.size() > 0 ) {
		insert(argv);
		argv.clear();
		}

// 	putchar('\n');
	
	return size();
	}


int command_list::parse_from_argv(int argc, char** argv) {
	int s=0;
	while (s<argc) {
		int e = find_next_command( s+1, argc, argv );
		
		insert( s, e, argv, ( size() == 0 ) );
		// if ( commands.size() == 0 ) first command = global command line
		
		s = e;
		}
	return size();
	}
