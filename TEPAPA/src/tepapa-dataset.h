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

#ifndef __tepapa_dataset_h
#define __tepapa_dataset_h 1 

#include "iptr.h"
#include "var.h"
#include "samples.h"
#include "hashtable.h"
#include "tepapa-results.h"

#include <string>



struct TEPAPA_dataset: public __iptr_member {
	sample_list            sl;     // cases with score;
	TEPAPA_Results         rr;     // result register;
	
	TEPAPA_dataset(): __iptr_member() {}
	TEPAPA_dataset(const TEPAPA_dataset& ds); 
	
	bool save_csv(
		const string& p_output_filename,
		bool f_gen_arff_headers,
		bool f_do_csv_binary_outcome,
		bool f_write_case_id,
		bool f_numeric_attribute,
		const char* delimiter,
		const char* quote,
		const char* missing
		);

// 	size_t size() const { return rr.size() ; }
	};


struct feature_array {
	double          score;
	string          name_str; 
	
	binary_profile  mask_numeric;
	binary_profile  bp;
	vector<double>  vp;
	
	void resize(size_t  size) {
		mask_numeric.resize(size);
		bp.resize(size);
		vp.resize(size);
		}
	
	feature_array(){}
	feature_array(size_t size) { resize(size); }
	
	void load_from(const TEPAPA_dataset& ds, int sl_i) ;
	};


struct feature_arrays: public vector<feature_array> {
	vector< iptr<pattern> > patterns;
	
	public:
	feature_arrays(): vector<feature_array> (){}
	feature_arrays(const TEPAPA_dataset& ds): vector<feature_array> () {
		load_from(ds); 
		} 
	
	void load_from(const TEPAPA_dataset& ds) ;
	bool has_numeric_features() const ;
	};

#endif // __tepapa_dataset_h 
