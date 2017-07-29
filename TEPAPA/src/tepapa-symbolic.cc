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

#include <stdlib.h>
#include <unistd.h>
#include "tepapa-symbolic.h"
#include "stopwatch.h"
#include "tepapa-msg.h"

#include "config.h"

int TEPAPA_Discoverer_Symbolic::traverse_ngrams_and_classes(
		TEPAPA_Evaluator& e,
		const token_string& ref, 
		token_string::const_iterator start, 
		int n, 
		const ngram_pattern& stack,
		const binary_profile& search_mask
	) {
	sample_list& sl = *sl_ptr;
	ngram_pattern rp = stack;
	rp.set_deep_cmp(f_deep_cmp);
	rp.set_use_wildcard(f_use_wildcard);
	
	const token& t = *start;
	
	token_string::const_iterator next = start + 1;
	
	int z = rp.size();
	
// 	if ( z ) fprintf(stderr, "\e[0;31m%s\e[0m %s  \e[1;33m%s\e[0m\n", search_mask.digest().c_str(), rp.to_string().c_str(), ght [ start->primary() ] );
	
	rp.resize( z + 1 );
	
	
	int cnt = 0;
	


	
	for (token::const_iterator i = t.begin(); i!=t.end(); ++i) {
		binary_profile search_mask_new = search_mask;

#if TEPAPA_MULTITHREAD
		discoverer_symbolic_mutex.lock();
#endif // TEPAPA_MULTITHREAD

		if ( sl.token_registry[*i].size() ) {
			search_mask_new &= sl.token_registry[*i] ;
			}
		else {
			msgf(VL_WARNING, "Token not yet evaluated: %s\t%d\t%d\t%s\t%s", __FUNCTION__, hr.exists(*i), (*i), ght[ (*i) ], search_mask_new.digest().c_str() );
			}
		
#if TEPAPA_MULTITHREAD
		discoverer_symbolic_mutex.unlock();
#endif // TEPAPA_MULTITHREAD
		
		rp[z] = *i;
		
		if ( ! hr.exists(*i) )  {
			hash_value h = *i;
			ngram_pattern  rpi;
			rpi.push_back(h);
			rpi.set_deep_cmp(f_deep_cmp);
			rpi.set_use_wildcard(f_use_wildcard);
			
			string substr_str_rpi = rpi.to_string();

			binary_profile vbrpi ;

#if TEPAPA_MULTITHREAD
			discoverer_symbolic_mutex.lock();
#endif // TEPAPA_MULTITHREAD

			const binary_profile&  bp_tri = sl.token_registry[*i] ;

#if TEPAPA_MULTITHREAD
			discoverer_symbolic_mutex.unlock();
#endif // TEPAPA_MULTITHREAD
		
			if ( bp_tri.size() ) {
				vbrpi = rpi.match(sl, bp_tri);
				}
			else {
				vbrpi = rpi.match(sl);
				}

// 			fprintf(stderr, "BPROF %s\t%s\n", ght[ (*i) ], vbrpi.digest().c_str() );

			unsigned int nprpi = vbrpi.npos();
			
			hr[h].set(nprpi); // mark as evaluated and record stats
// 			fprintf (stderr, "nprpi=%d\n", nprpi);
			
			if ( (nprpi > 1)  ) { // && (nprpi < vbrpi.size())
				iptr<pattern> pp = new ngram_pattern( rpi );
				
				e.eval_symbolic( vbrpi, v_score, pp );
				
				++cnt;
				}
			}


		string ngram_str = rp.to_string();
		
		hash_value h = ght( ngram_str.c_str() ) ;
		
		bool f_lookup_next_element = true;

// 		fprintf (stderr, "| %d\t%d\t%s\t%s\n", 
// 			hr.exists(h), (hr.exists(h) ? hr[h].npos : -1), 
// 			search_mask_new.digest().c_str(), ngram_str.c_str()
// 			);
		
// 		if ( ! hr.exists(h) ) {
		if ( hr.grab(h) ) { // i.e. not already computed
		
			hr.set_existence(h); // mark as evaluated
			
			binary_profile vb = rp.match(sl, search_mask_new);

// 			fprintf(stderr, "\e[1;34m%s %s\e[0m\n", vb.digest().c_str(), rp.to_string().c_str());

			search_mask_new = vb;
		
			unsigned int np = vb.npos();
			hr[h].npos = np;
// 			hr[h].bprof = vb;
			
			if ( (np > 1)  ) { // && (np < vb.size())
				iptr<pattern> pp = new ngram_pattern( rp );
				
				e.eval_symbolic( vb, v_score, pp );
				
				++cnt;
				}
			else {
				f_lookup_next_element = false;
				}
			
			hr.release(h);
			}
		else {
			int np = hr[h].npos;
			
// 			search_mask_new = hr[h].bprof;
// 			printf("np=%d\n", np);
 			if ( (np <= 1) ) {  // if ( (np <= 1) || ( np >= sl.size() ) ) {
				f_lookup_next_element = false;
				}
			}
		
		if ( f_lookup_next_element ) {
			if ( ( n <=0 ) || ( next == ref.end() ) || ( hv_separators.find( next->primary() ) != hv_separators.end() )  ) {
				f_lookup_next_element = false; 
				}	
			}
		
		if ( f_lookup_next_element ){
// 			fprintf (stderr, "  >> %d\t%d\t%s\t%s\n", 
// 				hr.exists(h), (hr.exists(h) ? hr[h].npos : -1), 
// 				search_mask_new.digest().c_str(), ngram_str.c_str()
// 				);
			
			cnt += traverse_ngrams_and_classes(e, ref, next, n - 1, rp, search_mask_new);
			
			if (f_use_wildcard) { // wildcard (gaps) -- A B C -> A B C <TOKEN> E
				token_string::const_iterator next_of_next = next + 1;
				
				// only add wildcard if the next of next token is valid
				if ( ( stack.size() ) && ( n > 1 ) && ( next_of_next != ref.end() ) && ( hv_separators.find( next_of_next->primary() ) == hv_separators.end() )  ) {
					ngram_pattern rp_wc_tail = stack;
					
					rp_wc_tail.push_back( hv_wildcard );
					
					cnt += traverse_ngrams_and_classes(e, ref, next_of_next, n - 2, rp_wc_tail, search_mask_new);
					}
				}
			}
		
		if (! f_deep_cmp) break;
		}
	
	return cnt;
	}


int TEPAPA_Discoverer_Symbolic::traverse_ngrams_and_classes(
	TEPAPA_Evaluator& e, const token_string& ref, token_string::const_iterator start, int n
	) {
	sample_list& sl = *sl_ptr;
	ngram_pattern stack;
	stack.set_deep_cmp(f_deep_cmp);
	stack.set_use_wildcard(f_use_wildcard);
	
	
	binary_profile  search_mask_true( sl.size() );
	search_mask_true = true;
	return traverse_ngrams_and_classes(e, ref, start, n - 1, stack, search_mask_true);
	}


#include <thread>


void TEPAPA_Discoverer_Symbolic::run1(int i) {
	sample_list& sl = *sl_ptr;
	
	const token_string& s = sl[i].data;
	
	for (token_string::const_iterator j=s.begin(); j!=s.end(); ++j) {
		if ( ! j->size() ) continue;

		if ( hv_separators.find( j->primary() ) != hv_separators.end() ) continue;
		
		int cnt_cls = traverse_ngrams_and_classes(*evaluator, s, j, ngram);
		
		total_cnt_cls  += cnt_cls ;
		}
	
	}


////////////////////////////////////////////////////////////////////
