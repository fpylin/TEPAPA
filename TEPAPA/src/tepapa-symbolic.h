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

#ifndef __tepapa_symbolic_h
#define __tepapa_symbolic_h 1

#include "hashtable.h"
#include "ngram-pattern.h"
#include "ngram-registry.h"
#include "samples.h"

#include "tepapa-discoverer.h"
#include "tepapa-evaluator.h"
#include "tepapa-gvar.h"


using namespace std;

#include <vector>
#include <set>

extern set<hash_value> separators;


class TEPAPA_Discoverer_Symbolic: public TEPAPA_Discoverer {
	int traverse_ngrams_and_classes(TEPAPA_Evaluator& e, const token_string& ref, token_string::const_iterator start, int n) ;
	int traverse_ngrams_and_classes(TEPAPA_Evaluator& e, const token_string& ref, token_string::const_iterator start, int n, const ngram_pattern& stack, const binary_profile& search_mask);

	bool f_deep_cmp;
	bool f_use_wildcard;
	int  ngram ;

	protected:
	virtual void run1(int i) ;
	std::mutex  discoverer_symbolic_mutex;
	
	public:
	TEPAPA_Discoverer_Symbolic(VariableList& param): TEPAPA_Discoverer(param) {
		f_use_wildcard = int( param[TEPAPA_ARGSTR_DISCOVERER_USE_WILDCARD] );
		f_deep_cmp = int( param[TEPAPA_ARGSTR_DISCOVERER_USE_DEEP_CMP] );
		ngram = param[TEPAPA_ARGSTR_DISCOVERER_MAX_NGRAM];
		}

	virtual ~TEPAPA_Discoverer_Symbolic() {}
// 	virtual bool run() ;
	};


#include "tepapa-program.h"

class TEPAPA_Program_Discoverer_Symbolic: public TEPAPA_Program_Discoverer<TEPAPA_Discoverer_Symbolic> {
	
	protected: 
// 	virtual TEPAPA_Discoverer*  create_instance() { return new TEPAPA_Discoverer_Symbolic; }

	public:
	TEPAPA_Program_Discoverer_Symbolic(): TEPAPA_Program_Discoverer ("@DiscoverSymbolic") {}
	virtual TEPAPA_Program* spawn() const { return new TEPAPA_Program_Discoverer_Symbolic; }
	};

	
#endif
