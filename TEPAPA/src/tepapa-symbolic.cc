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


////////////////////////////////////////////////////////////////
int TEPAPA_Discoverer_Symbolic::traverse_ngrams_and_classes(
		TEPAPA_Evaluator& e,
		const token_string& ref, 
		token_string::const_iterator ref_start, 
		token_string::const_iterator ref_end, 
		int n, 
		const ngram_pattern& stack,
		const binary_profile& search_mask,
		bool f_subset,
		const binary_profile& initial_search_mask
	) {
	const sample_list& sl = *sl_ptr;
	ngram_pattern rp = stack;
	rp.set_deep_cmp(f_deep_cmp);
	rp.set_use_wildcard(f_use_wildcard);
	
	const token& t = *ref_start;
	
// 	fprintf(stderr, "\e[0;31m%s\e[0m %s  \e[1;33m%s\e[0m\n", search_mask.digest().c_str(), rp.to_string().c_str(), ght [ ref_start->primary() ] );
	
	token_string::const_iterator next = ref_start + 1;
	
	int z = rp.size();
	
	
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
// 		fprintf(stderr, "\e[0;32m%s\e[0m %s\n", sl.token_registry[*i].digest().c_str(), rp.to_string().c_str() );
// 		fprintf(stderr, "\e[0;35m%s\e[0m %s\n", search_mask_new.digest().c_str(), rp.to_string().c_str() );
		
		if ( ! hr.exists(*i) )  {
			hash_value h = *i;
// 			hr.grab(h);
			ngram_pattern  rpi;
			rpi.push_back(h);
			rpi.set_deep_cmp(f_deep_cmp);
			rpi.set_use_wildcard(f_use_wildcard);
			
			string substr_str_rpi = rpi.to_string();

			binary_profile vbrpi ;

#if TEPAPA_MULTITHREAD
			discoverer_symbolic_mutex.lock();
#endif // TEPAPA_MULTITHREAD

			const binary_profile&  bp_tri = sl.token_registry[*i];

#if TEPAPA_MULTITHREAD
			discoverer_symbolic_mutex.unlock();
#endif // TEPAPA_MULTITHREAD

			/* to delete --> 
			if ( bp_tri.size() ) {
			  binary_profile mmask = search_mask_new;
			  mmask &= bp_tri;
			  if (substr_mask.size()) {
			    vbrpi = rpi.match(sl, mmask, substr_mask); // if partial matching
			  }
			  else {
			    vbrpi = rpi.match(sl, mmask ); // default, all corpus are included
			  }
			}
			else {
			  if (substr_mask.size()) {
			    vbrpi = rpi.match(sl, search_mask_new, substr_mask); // if partial matching
			  }
			  else {
			    vbrpi = rpi.match(sl); // default
			  }
			  } <-- */
			vbrpi = rpi.match(sl); 

// 			fprintf(stderr, "BPROF %s\t%s\n", ght[ (*i) ], vbrpi.digest().c_str() );

			unsigned int nprpi = vbrpi.npos();
			
			hr[h].set(nprpi); // mark as evaluated and record number of positives
// 			fprintf (stderr, "nprpi=%d\n", nprpi);
			
// 			if ( (nprpi >= min_support) && (nprpi <= (vbrpi.size() - min_support)) ) { // for both classes 
			if ( (nprpi >= min_support) ) { // for both classes # && (nprpi <= (vbrpi.size() - min_support)) 
// 				fprintf (stderr, "!\n");
				iptr<pattern> pp = new ngram_pattern( rpi );
				
				if (f_subset) {
				  e.eval_symbolic( vbrpi, outvar_vectors_t(v_score, v_censored), init_search_mask, pp );
				}
				else {
				  e.eval_symbolic( vbrpi, outvar_vectors_t(v_score, v_censored), pp );
				}
				
				++cnt;
				}
// 			fprintf(stderr, "0 npos=%ld\t%s\n", nprpi, rp.to_string().c_str());

// 			hr.release(h);
			}


		string ngram_str = rp.to_string();
		
		hash_value h = ght( ngram_str.c_str() ) ;
		
		bool f_lookup_next_element = true;

// 		fprintf (stderr, "| %d\t%d\t%s\t%s\n", 
// 			hr.exists(h), (hr.exists(h) ? hr[h].npos : -1), 
// 			search_mask_new.digest().c_str(), ngram_str.c_str()
// 			);
// 		fprintf(stderr, "1 f_lookup_next_element=%d\t%s\n", f_lookup_next_element, ngram_str.c_str());
		
// #if TEPAPA_MULTITHREAD
// 		discoverer_symbolic_mutex.lock();
// #endif // TEPAPA_MULTITHREAD

// #if TEPAPA_MULTITHREAD
// 		discoverer_symbolic_mutex.unlock();
// #endif // TEPAPA_MULTITHREAD

// 		if ( ! hr.exists(h) ) {
		if ( hr.grab(h) ) { // i.e. not already computed
		
			hr.set_existence(h); // mark as evaluated
			
			binary_profile vb = rp.match(sl, search_mask_new);
// 			binary_profile vb = rp.match(sl);

// 			fprintf(stderr, "\e[1;34mSM %s %s\e[0m\n", search_mask_new.digest().c_str(), rp.to_string().c_str());
// 			fprintf(stderr, "\e[1;34mVB %s %s\e[0m\n", vb.digest().c_str(), rp.to_string().c_str());
// 			fprintf(stderr, "\e[1;34mVB:%s %s\e[0m\n", vb1.digest().c_str(), rp.to_string().c_str());

			search_mask_new = vb;
		
			unsigned int np = vb.npos();
			hr[h].npos = np;
// 			hr[h].bprof = vb;
			
			
			if ( (np >= min_support)  ) { // && (np < vb.size())
// 				fprintf(stderr, "1-f_lookup_next_element=%d\tnpos=%ld\t%s\n", f_lookup_next_element, np, ngram_str.c_str());
				iptr<pattern> pp = new ngram_pattern( rp );
				
				if (f_subset) {
				  e.eval_symbolic( vb, outvar_vectors_t(v_score, v_censored), init_search_mask, pp );
				}
				else {
				  e.eval_symbolic( vb, outvar_vectors_t(v_score, v_censored), pp );
				}
				
				
				++cnt;
				}
			else {
				f_lookup_next_element = false;
				}
			
			hr.release(h);
			}
		else {
			unsigned int np = hr[h].npos;
			
// 			fprintf(stderr, "1+f_lookup_next_element=%d\tnpos=%ld\t%s\n", f_lookup_next_element, np, ngram_str.c_str());
			
// 			search_mask_new = hr[h].bprof;
// 			printf("np=%d\n", np);
 			if ( (np < min_support) ) {  // if ( (np <= 1) || ( np >= sl.size() ) ) {
				f_lookup_next_element = false;
				}
			}

// 		fprintf(stderr, "2 f_lookup_next_element=%d\t%s\n", f_lookup_next_element, ngram_str.c_str());

		if ( f_lookup_next_element ) {
			if ( ( n <=0 ) || ( next == ref_end ) || ( hv_separators.find( next->primary() ) != hv_separators.end() )  ) {
				f_lookup_next_element = false; 
				}	
			}

// 		fprintf(stderr, "3 f_lookup_next_element=%d\t%s\n", f_lookup_next_element, ngram_str.c_str());
		
		if ( f_lookup_next_element ){
// 			fprintf (stderr, "  >> %d\t%d\t%s\t%s\n", 
// 				hr.exists(h), (hr.exists(h) ? hr[h].npos : -1), 
// 				search_mask_new.digest().c_str(), ngram_str.c_str()
// 				);
			
		  cnt += traverse_ngrams_and_classes(e, ref, next, ref_end, n - 1, rp, search_mask_new, f_subset, init_search_mask);
			
			if (f_use_wildcard) { // wildcard (gaps) -- A B C -> A B C <TOKEN> E
				token_string::const_iterator next_of_next = next + 1;
				
				// only add wildcard if the next of next token is valid
				if ( ( stack.size() ) && ( n > 1 ) && ( next_of_next != ref_end ) && ( hv_separators.find( next_of_next->primary() ) == hv_separators.end() )  ) {
					ngram_pattern rp_wc_tail = stack;
					
					rp_wc_tail.push_back( hv_wildcard );
					
					cnt += traverse_ngrams_and_classes(e, ref, next_of_next, ref_end, n - 2, rp_wc_tail, search_mask_new, f_subset, init_search_mask);
					}
				}
			}
		
		if (! f_deep_cmp) break;
		}
	
	return cnt;
	}


////////////////////////////////////////////////////////////////
int TEPAPA_Discoverer_Symbolic::traverse_ngrams_and_classes(
  TEPAPA_Evaluator& e, const token_string& ref,
  token_string::const_iterator start,
  token_string::const_iterator end,
  int n
  ) {
  
  ngram_pattern stack;

  stack.set_deep_cmp(f_deep_cmp);
  stack.set_use_wildcard(f_use_wildcard);
  
  
  if ( init_search_mask.size() ) {
    binary_profile  search_mask = init_search_mask;
    return traverse_ngrams_and_classes(e, ref, start, end, n - 1, stack, search_mask, true, search_mask);
  }
  else {
    const sample_list& sl = *sl_ptr;
    binary_profile  search_mask_true( sl.size() );
    search_mask_true = true;
    return traverse_ngrams_and_classes(e, ref, start, end, n - 1, stack, search_mask_true, false, search_mask_true);
  }
}



////////////////////////////////////////////////////////////////
void TEPAPA_Discoverer_Symbolic::run1(int i) {
  const sample_list& sl = *sl_ptr;
	
  const token_string& ts = sl[i].data;

  if (! ts.size() ) return;
  
  matching_range  r(ts);
  
  if ( substr_mask.size() ) {
    assert(substr_mask.size() == sl.size());	  
    r = substr_mask[i];
  }
  
  token_string::const_iterator s =  (ts.begin() + r.from);
  token_string::const_iterator e =  (ts.begin() + r.to);
  if (r.from > ts.size()) {
    printf("%d\t%d\n", r.from, ts.size());
  }
  assert(r.from <= ts.size());
  assert(r.from >= 0);
  assert(r.to <= ts.size());
  assert(r.to >= 0);
	
  for (token_string::const_iterator j=s; j<e; ++j) {
    if ( ! j->size() ) continue;
    
    if ( hv_separators.find( j->primary() ) != hv_separators.end() ) continue;
    
    int cnt_cls = traverse_ngrams_and_classes(*evaluator, ts, j, e, ngram);
    
    total_cnt_cls  += cnt_cls ;
  }  
}


