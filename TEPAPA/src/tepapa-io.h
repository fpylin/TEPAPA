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

#ifndef __tepapa_io_h
#define __tepapa_io_h 1

#include "tepapa.h"
#include "tepapa-gvar.h"


#include "tepapa-program.h"

class TEPAPA_Program_MergeDatasets: public TEPAPA_Program {
	string          ds_name_output;
	vector<string>  ds_names_input;
	
	protected:
	virtual bool handle_argv(const vector<string>& argv) ;
	
	virtual int run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) ;
	
	public:
	
	TEPAPA_Program_MergeDatasets(): TEPAPA_Program("@MergeDatasets") {  }
	virtual TEPAPA_Program* spawn() const { return new TEPAPA_Program_MergeDatasets; }
	};


class TEPAPA_Program_CopyDataset: public TEPAPA_Program {
	bool    f_do_expand;
	string  ds1_name ;
	string  ds2_name ;
	
	protected:
	virtual bool handle_argv(const vector<string>& argv) ;
	
	public:
	TEPAPA_Program_CopyDataset();
	
	virtual int run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) ;
	virtual TEPAPA_Program* spawn() const { return new TEPAPA_Program_CopyDataset; }
	};


class TEPAPA_Program_Transfer: public TEPAPA_Program {
	protected:
	
	public:
	TEPAPA_Program_Transfer();
	
	virtual int run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) ;
	virtual TEPAPA_Program* spawn() const { return new TEPAPA_Program_Transfer; }
	};



class TEPAPA_Program_Set: public TEPAPA_Program {
	bool f_do_expand;
	
	protected:
	virtual bool handle_argv(const vector<string>& argv) ;
	
	public:
	TEPAPA_Program_Set();
	virtual int run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) {return TEPAPA_RETVAL_SUCCESS | TEPAPA_RETVAL_TERMINUS; }
	virtual TEPAPA_Program* spawn() const { return new TEPAPA_Program_Set; }
	};

	
class TEPAPA_Program_Print: public TEPAPA_Program {
	string fmt;
	
	public:
	TEPAPA_Program_Print();
	
	virtual int run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) ;
	virtual TEPAPA_Program* spawn() const { return new TEPAPA_Program_Print; }
	};


class TEPAPA_Program_Message: public TEPAPA_Program {
	vector<string>  message_list;
	public:
	TEPAPA_Program_Message(): TEPAPA_Program("@Message") {}
	
	virtual bool handle_argv(const vector<string>& argv) ;
	virtual int run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) ;
	virtual TEPAPA_Program* spawn() const { return new TEPAPA_Program_Message; }
	};

class TEPAPA_Program_UnlinkFile: public TEPAPA_Program {
	vector<string>  file_list;
	public:
	TEPAPA_Program_UnlinkFile(): TEPAPA_Program("@UnlinkFile") {}
	
	virtual bool handle_argv(const vector<string>& argv) ;
	virtual int run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) ;
	virtual TEPAPA_Program* spawn() const { return new TEPAPA_Program_UnlinkFile; }
	};

	
class TEPAPA_Program_PrintTwoClassStats: public TEPAPA_Program {
	public:
	TEPAPA_Program_PrintTwoClassStats(): TEPAPA_Program("@PrintTwoClassStats") {}
	
	virtual int run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) ;
	virtual TEPAPA_Program* spawn() const { return new TEPAPA_Program_PrintTwoClassStats; }
	};


class TEPAPA_Program_LoadTable: public TEPAPA_Program {
	string  csv_table_name;
	string  csv_file_name;
	
	protected: 
	virtual bool handle_argv(const vector<string>& argv) ;

	public:
	TEPAPA_Program_LoadTable();

	virtual int run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) ;
	virtual TEPAPA_Program* spawn() const { return new TEPAPA_Program_LoadTable; }
	};

class TEPAPA_Program_LoadCases: public TEPAPA_Program {
	string case_definition_file;
	
	protected: 
	virtual bool handle_argv(const vector<string>& argv) ;

	public:
	TEPAPA_Program_LoadCases(): TEPAPA_Program("@LoadCases") {}

	virtual int run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) ;
	virtual TEPAPA_Program* spawn() const { return new TEPAPA_Program_LoadCases; }
	};

class TEPAPA_Program_LoadCharstring: public TEPAPA_Program {
	string value_string_file;
	
	protected: 
	virtual bool handle_argv(const vector<string>& argv) ;

	public:
	TEPAPA_Program_LoadCharstring(): TEPAPA_Program("@LoadCharstring") {}

	virtual int run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) ;
	virtual TEPAPA_Program* spawn() const { return new TEPAPA_Program_LoadCharstring; }
	};


class TEPAPA_Program_GenerateCSV: public TEPAPA_Program {
	const char* delimiter;
	const char* quote;
	const char* missing;
	
	bool     f_gen_arff_headers;
	bool     f_do_csv_pairs;
	bool     f_write_case_id;
	bool     f_numeric_attributes;
	string   output_filename;
	string   template_dataset_name;
	
	protected: 
	virtual bool handle_argv(const vector<string>& argv) ;
	
	int save_dataset(const string& p_output_filename, TEPAPA_dataset&  ds) ;
	
	public:
	TEPAPA_Program_GenerateCSV();

	virtual int run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) ;
	virtual TEPAPA_Program* spawn() const { return new TEPAPA_Program_GenerateCSV; }
	};


#endif // __tepapa_results_io_h
