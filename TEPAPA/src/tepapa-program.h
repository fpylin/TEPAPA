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

#ifndef __tepapa_program_h
#define __tepapa_program_h  1

#include "iptr.h"
#include "tepapa-gvar.h"
#include "tepapa-results.h"

#include <functional>

template <typename FUNCTYPE>  struct TEPAPA_option: __iptr_member {
	string     name;    // Option name
	string     lname;   // Alternative / long name
	string     help;    // Help string
	string     arghelp; // Help string for the argument
	FUNCTYPE   func;    // Pointer to function
	char       vartype; // Type of variable 
	void*      varptr;  // Pointer to the variable
	
	TEPAPA_option(): __iptr_member() {}
	TEPAPA_option(
		const string& p_name, 
		const string& p_lname, 
		const string& p_help,
		char          p_vartype,
		void*         p_varptr,
		const string& p_arghelp,
		FUNCTYPE      p_func
		): __iptr_member() {
		name    = p_name ;
		lname   = p_lname ;
		help    = p_help;
		varptr  = p_varptr;
		vartype = p_vartype;
		arghelp = p_arghelp;
		func    = p_func;
		}
	};


struct TEPAPA_option_binary: public TEPAPA_option< std::function<bool()> > {
	static std::function<bool()>  default_func ;
	TEPAPA_option_binary(const string& p_name, const string& p_lname, const string& p_help, char p_vartype, void* p_varptr, const std::function<bool()>& p_func)
		: TEPAPA_option(p_name, p_lname, p_help, p_vartype, p_varptr, "", p_func) {}
	TEPAPA_option_binary(const string& p_name, const string& p_lname, const string& p_help, char p_vartype, void* p_varptr=0)
		: TEPAPA_option(p_name, p_lname, p_help, p_vartype, p_varptr, "", default_func) {}
	TEPAPA_option_binary(const string& p_name, const string& p_lname, const string& p_help, const std::function<bool()>& p_func)
		: TEPAPA_option(p_name, p_lname, p_help, 0, 0, "", p_func) {}
	};

struct TEPAPA_option_optarg: public TEPAPA_option< std::function<bool(const std::string&)> > {
	void set_default_arghelp() ;

	static std::function<bool(const std::string&)> default_func ;
	TEPAPA_option_optarg(const string& p_name, const string& p_lname, const string& p_help, char p_vartype, void* p_varptr, const std::function<bool(const std::string&)>& p_func)
		: TEPAPA_option(p_name, p_lname, p_help, p_vartype, p_varptr, "", p_func) {
		set_default_arghelp();
		}
	TEPAPA_option_optarg(const string& p_name, const string& p_lname, const string& p_help, char p_vartype, void* p_varptr=0)
		: TEPAPA_option(p_name, p_lname, p_help, p_vartype, p_varptr, "", default_func) {
		set_default_arghelp();
		}
	TEPAPA_option_optarg(const string& p_name, const string& p_lname, const string& p_help, const std::function<bool(const std::string&)>& p_func)
		: TEPAPA_option(p_name, p_lname, p_help, 0, 0, "", p_func) {
		set_default_arghelp();
		}
	};
	

/////////////////////////////////////////////////////////


class TEPAPA_Program: public __iptr_member {
	int step_no;
	int args_find_next_option(const vector<string>& args, int start) ;
	
	static int total_runs;

	virtual bool handle_options_binary(const string& opt) final ;
	virtual bool handle_options_optarg(const string& opt, const string& optarg) final ;
	
	string  name;
	string  active_dataset_name;  // active or training set
	string  test_dataset_name;    // test set
	string  export_dataset_name;  // dataset to export results to
	bool    f_export_append;      // default is FALSE
	bool    f_pipe_results;       // default is TRUE
	
	protected:
	vector<TEPAPA_option_binary> options_binary;
	vector<TEPAPA_option_optarg> options_optarg;
	
	TEPAPA_global_data_struct*   gvr_ptr; 
	VariableList                 param;        // parameter list (with value)
	
// 	string                       extra_optstring;
// 	virtual int  handle_options(const string& opt, const vector<string>& sub_args) final { abort(); }
// 	virtual bool handle_option(char c, const string& optarg) final { abort(); } ;

	virtual bool handle_argv(const vector<string>& argv) { return true; }
	
	virtual void initialise(void) {}

	virtual int run() { abort(); return TEPAPA_RETVAL_SUCCESS; }
	
	virtual int run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) = 0; // { abort(); return TEPAPA_RETVAL_SUCCESS; }
	
	string try_expand_variable(const string& varstr) {
		assert(gvr_ptr);
		TEPAPA_global_data_struct&  gvr = * gvr_ptr;
		return gvr.variables.try_expand(varstr);
		}
		
	public:
	
	TEPAPA_Program(const string& pname) ;
	
	void set_global_data(TEPAPA_global_data_struct& gvr_ref) { gvr_ptr = &gvr_ref; }
	void set_step_no(int s) { step_no = s; }
	void set_default_datasets_name() ;
	
	TEPAPA_dataset&  get_active_dataset () ;
	TEPAPA_dataset&  get_export_dataset () ;
	TEPAPA_dataset&  get_test_dataset () ;

	const string& getname() const { return name; }
	
// 	vector<string> argv_to_vector(int argc, char** argv);

	virtual bool process_command_line(const vector<string>& args) ;
	
	virtual int run(TEPAPA_global_data_struct& gvr, const vector<string>& args) ;
	
	virtual TEPAPA_Program* spawn() const = 0;
	
	string get_help_message() const;
// 	int run(TEPAPA_global_data_struct& gvr, int argc, char** argv) ;
	};


#endif // __tepapa_program_h
