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

#ifndef __tepapa_numeric_h
#define __tepapa_numeric_h 1

#include "tepapa-discoverer.h"
#include "ngram-pattern.h"

class TEPAPA_Discoverer_Numeric: public TEPAPA_Discoverer {
	int total_cnt_cls;

	std::mutex  discoverer_numeric_mutex;

	bool f_deep_cmp;
	bool f_use_wildcard;
	int  ngram ;
	
	protected:
	int traverse_ngram_classes_spearman(vector<extractable_ngram_pattern>& out_rpl, hash_value index_class, token_string::const_iterator index, int ka, int kb);
	virtual void run1(int i) ;
	
	public:
	TEPAPA_Discoverer_Numeric(VariableList& param): TEPAPA_Discoverer(param) {
		f_use_wildcard = int( param[TEPAPA_ARGSTR_DISCOVERER_USE_WILDCARD] );
		f_deep_cmp = int( param[TEPAPA_ARGSTR_DISCOVERER_USE_DEEP_CMP] );
		ngram = param[TEPAPA_ARGSTR_DISCOVERER_MAX_NGRAM];
		}

	virtual ~TEPAPA_Discoverer_Numeric() {}
// 	virtual bool run() ;
	};


#include "tepapa-program.h"

class TEPAPA_Program_Discoverer_Numeric: public TEPAPA_Program_Discoverer<TEPAPA_Discoverer_Numeric> {
	protected: 
	public:
	TEPAPA_Program_Discoverer_Numeric(): TEPAPA_Program_Discoverer ("@DiscoverNumeric") {}
	virtual TEPAPA_Program* spawn() const { return new TEPAPA_Program_Discoverer_Numeric; }
	};

	
#endif // __tepapa_numeric_h 
