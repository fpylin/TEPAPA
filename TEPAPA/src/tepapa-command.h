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
#ifndef __tepapa_command_h
#define __tepapa_command_h 1

#define COMMAND_NAME_MAIN   "@Main"

#include <string>
#include "iptr.h"
#include "tepapa-program.h"


class program_list: public map<std::string, iptr<TEPAPA_Program> >  {
	
	public:
	iptr<TEPAPA_Program> find_program(const string& str) const ;
		
	void insert(TEPAPA_Program* pp) ;
	};

	
struct command_structure {
	iptr<TEPAPA_Program>   program;
	vector<string>         args;
	double                 elapsed_sec; 
		
	void set_primary_program(const program_list&  pl) ;

	void set(const program_list&  pl, int p_argi_begin, int p_argi_end, char** p_argv, bool is_tepapa_main=false) ;
	
	void set(const program_list&  pl, const vector<string>& strs, bool is_tepapa_main=false) ;
	
	string to_string(const char* delim="\t") const ;

	command_structure() { elapsed_sec=0 ;}
	
	~command_structure() {}

	command_structure(const program_list&  pl, const vector<string>& strs) { 
		set(pl, strs); 
		}
		
	command_structure(const program_list&  pl, int p_argi_begin, int p_argi_end, char** p_argv, bool is_tepapa_main=false) { 
		set(pl, p_argi_begin, p_argi_end, p_argv, is_tepapa_main); 
		}
	};

	
class command_list: public vector<command_structure> {
	const program_list*  pl_ptr;

	int find_next_command(int i, int argc, char **argv) ;
	
	public:
	void set_program_list(const program_list& pl_ref) { pl_ptr = &pl_ref; }
	
	command_list(); 
	command_list(const program_list& pl_ref) ;
	
	
	int parse_from_file(const string& filename) ;
	
	int parse_from_argv(int argc, char** argv) ;
	
	void insert(const vector<string>& argv) ;
		
	void insert(int p_argi_begin, int p_argi_end, char** p_argv, bool is_tepapa_main=false) ;
	};

#endif // __tepapa_command_h
