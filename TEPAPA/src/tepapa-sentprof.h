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

#ifndef __tepapa_sentprof_h
#define __tepapa_sentprof_h 1

#include "tepapa-program.h"
#include "tepapa-discoverer.h"
#include "sentence-pattern.h"
#include "ngram-registry.h"

template <class CHUNKTYPE> class TEPAPA_Discoverer_ChunkProfile: public TEPAPA_Discoverer {
	
	vector< vector<CHUNKTYPE> >  vvrs ;
	
	simple_hash_registry  shr; 
	
	int dissimilarity_metric;
	
	protected:
	virtual void initialise() ;
	virtual void run1(int i) ;

	void set_dissimilarity_metric(int p_dissimilarity_metric) { dissimilarity_metric = p_dissimilarity_metric; }
	
	
	public:
	TEPAPA_Discoverer_ChunkProfile(VariableList& param): TEPAPA_Discoverer(param) { 
		dissimilarity_metric = 0;  // i.e. default, can be ignored
		shr.set_hash_table(ght);
		}
		
	virtual ~TEPAPA_Discoverer_ChunkProfile() {}
	} ;


	
template <class CHUNKTYPE> void TEPAPA_Discoverer_ChunkProfile<CHUNKTYPE>::run1(int i) {
	unsigned int dim=vvrs.size();
	
	for(unsigned int x=0; x<vvrs[i].size(); ++x) {
		binary_profile bp (dim);
		vector<double> vs (dim);
		
		const string s = vvrs[i][x].to_string().c_str();
		hash_value hv = ght( s.c_str() ) ;
		
// 		printf("%lu\t%s\n", hv, s.c_str());

		if (shr.exists(hv) )  continue;
		shr.set_existence( hv );
		
		
		for(unsigned int j=0; j<dim; ++j) {
			double min_v = 9e99;
// 			if (i == j) {
// 				bp[j] = true;
// 				vs[j] = 0;
// 				continue;
// 				}
			for(unsigned int y=0; y<vvrs[j].size(); ++y) {
				double v = CHUNKTYPE::calc_dissimilarity_metric(vvrs[i][x], vvrs[j][y], dissimilarity_metric); 
// 				printf("i=%d\tx=%d\t%s\nj=%d\ty=%d\t%s\nv=%g\nmin_v=%g\n", i, x, vvrs[i][x].to_string().c_str(), j, y, vvrs[j][y].to_string().c_str(), v, min_v);
				if ( v < min_v ) min_v = v;
				}
			bp[j] = true;
			vs[j] = 1.0 - min_v;
// 			printf("min_v(%d,%d)=%g\n", i, j, min_v);
			
			}
			
		iptr<pattern> pp = new CHUNKTYPE( vvrs[i][x] );
		
		evaluator -> eval_numeric( bp, v_score, vs, pp );
		}
	}


template <class CHUNKTYPE> void TEPAPA_Discoverer_ChunkProfile<CHUNKTYPE>::initialise() {
	sample_list& sl = *sl_ptr;
	
	vvrs.clear() ;

	for(unsigned int i=0; i<sl.size(); ++i) {
		vector<CHUNKTYPE> vrs = CHUNKTYPE::convert_from(sl[i].data);
		vvrs.push_back( vrs );
		}
	}


	
	
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	

#include "bowsent.h"

typedef TEPAPA_Discoverer_ChunkProfile<bag_of_words_sentence_pattern>  TEPAPA_Discoverer_BOWSentenceProfile;

class TEPAPA_Program_Discoverer_BOWSentenceProfile: public TEPAPA_Program_Discoverer< TEPAPA_Discoverer_BOWSentenceProfile > {
	public:
	TEPAPA_Program_Discoverer_BOWSentenceProfile(): 
		TEPAPA_Program_Discoverer< TEPAPA_Discoverer_BOWSentenceProfile >("@DiscoverBOWSentenceProfile") {
		}
	
	virtual TEPAPA_Program* spawn() const { 
		return new TEPAPA_Program_Discoverer_BOWSentenceProfile(); 
		}
	};

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////	
#include "sentence-pattern.h"
typedef TEPAPA_Discoverer_ChunkProfile<sentence_pattern>  TEPAPA_Discoverer_SentenceProfile;

class TEPAPA_Program_Discoverer_SentenceProfile: public TEPAPA_Program_Discoverer< TEPAPA_Discoverer_SentenceProfile > {
	public:
	TEPAPA_Program_Discoverer_SentenceProfile(): 
		TEPAPA_Program_Discoverer< TEPAPA_Discoverer_SentenceProfile >("@DiscoverSentenceProfile") {}
	
	virtual TEPAPA_Program* spawn() const { 
		return new TEPAPA_Program_Discoverer_SentenceProfile(); 
		}
	};


#endif //  __tepapa_sentprof_h
