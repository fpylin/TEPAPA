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
#include "tepapa-gvar.h"
#include "tepapa-msg.h"
#include "parse.h"
#include <string.h>

TEPAPA_dataset::TEPAPA_dataset(const TEPAPA_dataset& ds): __iptr_member() { 
	sl=ds.sl;
	rr=ds.rr; 
	}


string VariableList::try_expand(const string& s) { 
	char* z;
	char* buf = new char[ s.length() + 65536 ];
	strcpy(buf, s.c_str());
	
	char* t = buf;
	
	string retval;
	
	while ( (z = strchr(t, '$')) ) {
		char* y = z + 1;
		while( y && *y && ( isalnum(*y) || *y=='_' ) ) ++y;
		int len = (y - z - 1);
		
		*z = 0;
		retval += t;

		char buf1[65536];
		strncpy(buf1, z+1, len);
		buf1[len] = 0;
// 		printf(">> %s\n", buf1);
		retval += operator[](buf1);
		
		t = y;
		}
	retval += t;
	
	delete[] buf;
	
	return retval; 
	}


void VariableList::debug_dump() const {
	const_iterator  it = begin();
	
	msgf(VL_INFO, "%8s\t%-32s\t%-50s\n", "Hash #", "Variable name",  "Value" );
	for(it=begin(); it!=end(); ++it) {
		msgf(VL_INFO, "%8lu\t%-32s\t%-50s\n", it->first, ght[ it->first ], it->second.c_str() );
		}
	}
	
void TEPAPA_global_data_struct::dump_datasets() const {
	
	map<hash_value, iptr<TEPAPA_dataset> >::const_iterator it = datasets.begin();
	
	msgf(VL_INFO, "%8s\t%-32s\t%-10s\t%-10s\n",  "Hash #", "Dataset name", "n(Samples)", "n(Features)" );
	for(it=datasets.begin(); it!=datasets.end(); ++it) {
		msgf(VL_INFO, "%8lu\t%-32s\t%10lu\t%10lu\n", 
			it->first, ght[ it->first ], 
			it->second->sl.size(), 
			it->second->rr.size() 
			);
		}
	}

