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

#ifndef __tepapa_resample_h
#define __tepapa_resample_h 1

#include "tepapa-gvar.h"
#include "tepapa-program.h"

class TEPAPA_Program_Resample: public TEPAPA_Program {
	int    n_iters ; 
	string action ;
	string label  ;
	
	protected: 
	string varname_iters        ;
	string varname_current_iter ;
	string varname_return_step  ;
	string varname_orig_dataset ;
	string training_set_name ;
	string testing_set_name ;
	

	virtual bool handle_argv(const vector<string>& argv) ;
	virtual void define_varname() ;
	
	int get_n_iters() const {return n_iters; }

	public:
	TEPAPA_Program_Resample(const string& pname);

	virtual unsigned int initialise(sample_list&  sl_original) =0;
	
	// out_sl_training and sl_testing are newly created 
	virtual unsigned int run_iteration(int iteration, sample_list&  out_sl_training, sample_list& out_sl_testing ) =0;
	
	virtual int run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) ;
	};



//////////////////////////////////////////////////////////////////////////////////

class TEPAPA_Program_CrossValidate: public TEPAPA_Program_Resample{	
	public:
	TEPAPA_Program_CrossValidate(): TEPAPA_Program_Resample("@CrossValidate") { }

	virtual unsigned int initialise(sample_list&  sl_original) ;
	virtual unsigned int run_iteration(int current_iter, sample_list&  out_sl_training, sample_list& out_sl_testing ) ;
	virtual TEPAPA_Program* spawn() const { return new TEPAPA_Program_CrossValidate; }
	};

//////////////////////////////////////////////////////////////////////////////////
	
class TEPAPA_Program_Bootstrapping: public TEPAPA_Program_Resample{
	public:
	TEPAPA_Program_Bootstrapping(): TEPAPA_Program_Resample("@Bootstrap") { }

	virtual unsigned int initialise(sample_list&  sl_original) { return sl_original.size(); }
	virtual unsigned int run_iteration(int current_iter, sample_list&  out_sl_training, sample_list& out_sl_testing ) ;
	virtual TEPAPA_Program* spawn() const { return new TEPAPA_Program_Bootstrapping; }
	};

//////////////////////////////////////////////////////////////////////////////////
	
class TEPAPA_Program_RandomSubsampling: public TEPAPA_Program_Resample{
	double r_training;
	public:
	TEPAPA_Program_RandomSubsampling();

	virtual unsigned int initialise(sample_list&  sl_original) { return sl_original.size(); }
	virtual unsigned int run_iteration(int current_iter, sample_list&  out_sl_training, sample_list& out_sl_testing ) ;
	virtual TEPAPA_Program* spawn() const { return new TEPAPA_Program_RandomSubsampling; }
	};


#endif // __tepapa_resample_h
