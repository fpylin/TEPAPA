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
#ifndef __tepapa_gvar_h
#define __tepapa_gvar_h 1

#include "iptr.h"
#include "var.h"
#include "samples.h"
#include "hashtable.h"
#include "tepapa-results.h"
#include "tepapa-dataset.h"
#include "tepapa.h"

#include <map>


	
class VariableList: public std::map<hash_value, Var>  {
	Var& operator[] (const hash_value  hv) {
		return map<hash_value, Var>::operator[](hv) ;
		}
		
	public:
	VariableList(): std::map<hash_value, Var> ()  {}
	
	Var& operator[] (const char* s) {
// 		printf("%s\n", s);
		if ( s[0] == '$' ) { // reference to a variable
			return operator[]( s + 1 );
			}
		else {
			return std::map<hash_value, Var>::operator[]( ght( s ) ) ;
			}
// 		fprintf(stderr, "%s\t%s\n", __PRETTY_FUNCTION__, varname.c_str() );
		}
	
	Var& operator[] (const std::string& s) {
// 		string varname = s;
// 		fprintf(stderr, "%s\t%s\n", __PRETTY_FUNCTION__, varname.c_str() );
		return operator[]( s.c_str() ) ;
		}
		
	string try_expand(const string& s) ;
	
	void debug_dump() const;
	};



#include "datatable.h"
	
class TEPAPA_global_data_struct {
	map<hash_value, iptr<TEPAPA_dataset> >  datasets;   // global datasets
	
	void create_dataset_if_empty(hash_value hv_set_id) {
		if ( datasets.find(hv_set_id) == datasets.end() ) {
			TEPAPA_dataset* d = new TEPAPA_dataset;
			datasets[ hv_set_id ] = d;
			}
		}
		
	TEPAPA_dataset& get_dataset(hash_value hv_set_id) {
		create_dataset_if_empty(hv_set_id);
		TEPAPA_dataset* ds = datasets[ hv_set_id ];
		return *ds;
		}


	TEPAPA_dataset& get_dataset(const char* dataset_name) {
		assert( dataset_name[0] );
		if ( dataset_name[0] == '$' ) { // reference to a variable
			string dataset_name1 = variables[ dataset_name  ];
			hash_value  hv_set_id = ght( dataset_name1.c_str() );
			return get_dataset( hv_set_id );
			}
		else {
			hash_value  hv_set_id = ght( dataset_name );
			return get_dataset( hv_set_id );
			}
		}

	public:
	VariableList                variables;   // global list of datasets
	map<hash_value, data_table> data_tables; 

	TEPAPA_dataset& get_dataset(const string& dataset_name) { 
		return get_dataset( dataset_name.c_str() );
		}

	string get_active_dataset_name() {
		return  variables[GVAR_ACTIVE_DATASET]; 
		}

	void set_active_dataset(const string& dataset_name) {
		variables[ GVAR_ACTIVE_DATASET ] = dataset_name;
// 		fprintf(stderr, "Setting GVAR_ACTIVE_DATASET to %s\n", variables[ GVAR_ACTIVE_DATASET ].c_str());
		}
	
	TEPAPA_global_data_struct() { }
	
	void dump_datasets() const ;
	};
	
extern TEPAPA_global_data_struct  TEPAPA_global_data;

#endif // __tepapa_gvar_h
