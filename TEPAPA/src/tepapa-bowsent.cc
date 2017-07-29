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

#include "tepapa-bowsent.h"
#include "parallel.h"
#include <unistd.h>
#include <mutex>
#include <future>

//////////////////////////////////////////////////////////////////////////////////////


// void gen_bow_sentence_patterns2(set<bag_of_words_sentence>& s_out, const vector< vector<bag_of_words_sentence> >&  vvrs, int istart) {
// 	// const vector< map<hash_value, vector<int> > >&  token_index, 
// 	fprintf(stderr, "Processing sample %u\n", istart);
// 	
// 	static std::mutex gen_bow_sentence_patterns2_mutex;
// 
// 	for(unsigned int x=0; x<vvrs[istart].size(); ++x) {
// 		const bag_of_words_sentence& rsix = vvrs[istart][x];
// 		
// 		for(unsigned int j=istart+1; j<vvrs.size(); ++j) {
// 			
// 			for(unsigned int y=0; y<vvrs[j].size(); ++y) {
// 				const bag_of_words_sentence& rsjy = vvrs[j][y];
// 	// 			printf("yj = %s\n", rsjy.to_string().c_str());
// 				
// 				bag_of_words_sentence rsc = rsix & rsjy ;
// 				
// 				if (! rsc.size()) continue;
// 				
// // 				gen_bow_sentence_patterns2_mutex.lock();
// 				s_out.insert( rsc );
// // 				gen_bow_sentence_patterns2_mutex.unlock();
// 				}
// 			}
// 		}
// 	}
	

void gen_bow_sentence_patterns2(set<bag_of_words_sentence>& s_out, const vector<bag_of_words_sentence>&  vbowsi) {
	for(unsigned int i=0; i<vbowsi.size(); ++i) {
		vector<bag_of_words_sentence>  ss = vbowsi[i].gen_multiplets(3);
// 		printf("%d\t%u\n", i, ss.size());
		for(unsigned int i=0; i<ss.size(); ++i ) s_out.insert( ss[i] );
		}
	}

	
unsigned int gen_bow_sentence_patterns(vector<bag_of_words_sentence>& v_out, const vector< vector<bag_of_words_sentence> >&  vvrs) {
// 	, const vector< map<hash_value, vector<int> > >&  token_index
	set<bag_of_words_sentence> s_out;
	vector< set<bag_of_words_sentence> > vs_out;
	vs_out.reserve( vvrs.size() );

#if TEPAPA_MULTITHREAD 
	thread_manager  tm;
#endif // TEPAPA_MULTITHREAD
	
	for(unsigned int i=0; i<vvrs.size(); ++i) {		
		fprintf(stderr, "Processing sample %u\n", i);
	
#if TEPAPA_MULTITHREAD 
		tm.queue(); 
		vs_out.push_back( set<bag_of_words_sentence>() );
// 		tm.push_back( std::thread( &gen_bow_sentence_patterns2, std::ref(vs_out[i]), std::ref(vvrs), i) ); // std::ref(token_index), 
		tm.push_back( std::async( std::launch::async, &gen_bow_sentence_patterns2, std::ref(vs_out[i]), std::ref(vvrs[i]) ) ); 
#else 
// 		gen_bow_sentence_patterns2(s_out, vvrs, i);
		gen_bow_sentence_patterns2(s_out, vvrs[i]);
#endif // TEPAPA_MULTITHREAD
		}
	
#if TEPAPA_MULTITHREAD 
	tm.join_all();
#endif // TEPAPA_MULTITHREAD
	
	for(unsigned int i=0; i<vs_out.size(); ++i) {
// 		printf("%d\t%u\n", i, vs_out[i].size());
		s_out.insert( vs_out[i].begin(), vs_out[i].end() );
		}
		
	v_out.insert(v_out.begin(), s_out.begin(), s_out.end());
	
	return v_out.size();
	}
	
	
	


void TEPAPA_Discoverer_BOWSentence::eval(const vector< vector<bag_of_words_sentence> >&  haystack, const vector<bag_of_words_sentence>& needles, const bag_of_words_sentence_index&  token_index, int start, int end) {
	
	msgf(VL_INFO, "Evaluating patterns %d to %d\n", start, end);
	
	TEPAPA_Evaluator& e = *evaluator;
	
	for(int i=start; i<end; ++i) {
		
		binary_profile bp = needles[i].has_pattern(haystack, token_index);
		
		unsigned int bownpos = bp.npos();
		
		if ( (bownpos > 1) && (bownpos < bp.size()) ) {		
			
			iptr<pattern> pp = new bag_of_words_sentence_pattern( needles[i] );
		
			e.eval_symbolic( bp, v_score, pp );
			}
		}	
	}


////////////////////////////////////////////////////////////



	
bool TEPAPA_Discoverer_BOWSentence::run() {
	sample_list& sl = *sl_ptr;
	
	vector< vector<bag_of_words_sentence> >  vvrs ;

	for(unsigned int i=0; i<sl.size(); ++i) {
		vvrs.push_back( bag_of_words_sentence::convert_from(sl[i].data) );
		}

	bag_of_words_sentence_index  token_index(vvrs);
		
	vector<bag_of_words_sentence>  v_rsc;
	
	unsigned int n = gen_bow_sentence_patterns(v_rsc, vvrs) ; // , token_index
	
	msgf(VL_INFO, "%u patterns generated.\n", n);
	
#if TEPAPA_MULTITHREAD 
	thread_manager  tm;
	
	int ndiv = int(v_rsc.size()) / 1000;
	
	for(int i=0; i<ndiv; ++i) {
		uint unit = int(v_rsc.size()) / ndiv;
		uint start = i * unit ;
		uint end = (i + 1) * unit ;
		if ( end > v_rsc.size() ) end = v_rsc.size();
		tm.queue(); 
		tm.push_back( std::async( std::launch::async, &TEPAPA_Discoverer_BOWSentence::eval, this, std::ref(vvrs), std::ref(v_rsc), std::ref(token_index), start, end) );
// 		tm.create( [&]() { eval(vvrs, v_rsc, token_index, start, end); } );
		}
	
	tm.join_all();
#else 
	eval( vvrs, v_rsc, 0, v_rsc.size() ); // token_index, 
#endif // TEPAPA_MULTITHREAD
	
	return true;
	}
