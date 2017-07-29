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

#ifndef __tepapa_bowsent_h
#define __tepapa_bowsent_h 1


#include "hashtable.h"
#include "samples.h"
#include "regex.h"
#include "tepapa-evaluator.h"
#include "tepapa-gvar.h"
#include "tepapa-discoverer.h"

#include "bowsent.h" 



	
////////////////////////////////////////////////////////////////////////////////////////

class TEPAPA_Discoverer_BOWSentence: public TEPAPA_Discoverer {
	protected:
	
	
	virtual void run1(int i) { }
// 	virtual binary_profile find_pattern(const vector< vector<bag_of_words_sentence> >&  haystack, const bag_of_words_sentence& needle) const ;
// 	virtual binary_profile find_pattern(const vector< vector<bag_of_words_sentence> >&  haystack, const bag_of_words_sentence& needle, 
// 		const bag_of_words_sentence_index& token_index) const;
	
// 	virtual void eval(
// 		const vector< vector<bag_of_words_sentence> >&  haystack, const vector<bag_of_words_sentence>& needles, 
// 		int start, int end) ;

	virtual void eval(
		const vector< vector<bag_of_words_sentence> >&  haystack, const vector<bag_of_words_sentence>& needles, 
		const bag_of_words_sentence_index&  token_index, int start, int end
		) ;
	
// 	const vector< map<hash_value, vector<int> > >&  token_index,  
	
	
	public:
	TEPAPA_Discoverer_BOWSentence(VariableList& param): TEPAPA_Discoverer(param) {
		}
	
	virtual ~TEPAPA_Discoverer_BOWSentence() {}

	virtual bool run() ;
	} ;



#include "tepapa-program.h"
	
class TEPAPA_Program_Discoverer_BOWSentence: public TEPAPA_Program_Discoverer<TEPAPA_Discoverer_BOWSentence> {
	public:
	TEPAPA_Program_Discoverer_BOWSentence(): TEPAPA_Program_Discoverer<TEPAPA_Discoverer_BOWSentence>("@DiscoverBOWSentence") {}
	
	virtual TEPAPA_Program* spawn() const { return new TEPAPA_Program_Discoverer_BOWSentence; }
	};


#endif // #ifndef __tepapa_bowsent_h
