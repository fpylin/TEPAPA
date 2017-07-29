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

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#include "hashtable.h"


hash_table  ght; // this is the global hash table



hash_value hash_table::hash_string(const char* str) {
	/* djb2 algorithm: reference needed */
	unsigned long hash = 5381;
	register int c ;

	while ( (c = *str++) )
		hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

	return hash;
	}


hash_table::hash_table() {
	hv_last = 0;
// 	table.reserve( 4194304 ); // HASH_TABLE_NUM_KEYS 
	table.reserve( 65536 ); // HASH_TABLE_NUM_KEYS 
	}



hash_table::~hash_table() {}


#include "tepapa-msg.h"

hash_value hash_table::hash(const string& str) {
	hash_value h = hash_string(str.c_str());
	
	unsigned int hkey = h % HASH_TABLE_NUM_KEYS;

#if TEPAPA_MULTITHREAD
	hash_table_mutex.lock();
#endif

	map<string,hash_value>::const_iterator cit = index[hkey].find(str);

// 	msgf(VL_DEBUG, "hkey group %d\t%s\n", hkey, str.c_str());
	hash_value hr = 0;
	
	if ( cit != index[hkey].end() )  {
		hr = cit->second;
		}
	else 
		{
		hr = hv_last;
	
#if TEPAPA_MULTITHREAD
		locked_resize(hr+1);
#else
		resize(hr+1);
#endif
		table[hr] = str;
		index[hkey][str] = hr; 
	
// 	msgf(VL_DEBUG, "%d\t%d\t[%s]\n", hr, index[hkey][str], table[hr].c_str() );
	
		++ hv_last;
		}
		
#if TEPAPA_MULTITHREAD
	hash_table_mutex.unlock();
#endif	
		
	return hr;
	}


void hash_table::debug_print() {
	for (int i=0; i<hv_last; ++i) {
		printf("%d\t%s\n", i, table[i].c_str());
		}
	}
	
