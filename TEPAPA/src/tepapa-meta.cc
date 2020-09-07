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
#include "tepapa-meta.h"
#include "tepapa-msg.h"
#include "ngram-pattern.h"
#include "samples.h"
#include "tepapa-results.h"

#include "fgroup.h"

#include <signal.h>

/* FIXME: Current BUGGY! DO NOT USE
 */

TEPAPA_Program_Discoverer_MetaPattern::TEPAPA_Program_Discoverer_MetaPattern() :
	TEPAPA_Program_Discoverer<TEPAPA_Discoverer_MetaPattern>("@DiscoverMetaPatterns") {
	
	param[TEPAPA_ARGSTR_DISCOVERER_META_PATTERN_DEPTH] = 5;
	
	options_optarg.push_back( 
		TEPAPA_option_optarg (TEPAPA_ARGSTR_DISCOVERER_META_PATTERN_DEPTH, "--depth", "", 'i')
		);
	}


MPL_retval_struct TEPAPA_Discoverer_MetaPattern::MPL_AND(const TEPAPA_Result& r1, const TEPAPA_Result& r2) {
	MPL_retval_struct  retval;
	retval.bprof_out  = r1.bprof;
	retval.bprof_out &= r2.bprof;
	
	if ( retval.bprof_out == r1.bprof ) return retval;
	if ( retval.bprof_out == r2.bprof ) return retval;
		
	if ( retval.bprof_out.npos() <= 1 || retval.bprof_out.npos() >= retval.bprof_out.size() - 1 ) return retval;
	
	meta_pattern* mpp = new meta_pattern( ght("AND") );;
	mpp->push_back( r1.patt->clone() );
	mpp->push_back( r2.patt->clone() );
	retval.mp = mpp;
	
	return retval;
	}



MPL_retval_struct TEPAPA_Discoverer_MetaPattern::MPL_FOLLOW(const sample_list& sl, const TEPAPA_Result& r1, const TEPAPA_Result& r2) {
	MPL_retval_struct  retval;
	retval.bprof_out  = r1.bprof;
	retval.bprof_out &= r2.bprof;
	
	if ( retval.bprof_out == r1.bprof ) return retval;
	if ( retval.bprof_out == r2.bprof ) return retval;
		
	if ( retval.bprof_out.npos() <= 1 || retval.bprof_out.npos() >= retval.bprof_out.size() - 1 ) return retval;
	
	for(unsigned int i=0; i<sl.size(); ++i) {
		if ( ! retval.bprof_out[i] )  continue;
		pattern* pp1 = r1.patt ;
		ngram_pattern* ngp1 = dynamic_cast<ngram_pattern*>(pp1);
		if (! ngp1) {
			retval.bprof_out[i] = false;
			continue;
			}
		
		pattern* pp2 = r2.patt ;
		ngram_pattern* ngp2 = dynamic_cast<ngram_pattern*>(pp2);
		if (! ngp2) {
			retval.bprof_out[i] = false;
			continue;
			}
		
		vector<size_t> vi1 = ngp1->find_all( sl[i].data );
		vector<size_t> vi2 = ngp2->find_all( sl[i].data );
		
		bool f_found = false;
		for(unsigned int j1=0; j1<vi1.size(); j1++) {
			for(unsigned int j2=0; j2<vi2.size(); j2++) {
				if (vi1[j1] < vi2[j2]) {
					f_found = true;
					break;
					}
				if (f_found) break;
				}
			if (f_found) break;
			}
		if (! f_found) retval.bprof_out[i] = false;
		}

	retval.mp = new meta_pattern( ght("FOLLOW") );
	retval.mp->push_back( r1.patt );
	retval.mp->push_back( r2.patt );
	
	return retval;
	}

MPL_retval_struct TEPAPA_Discoverer_MetaPattern::MPL_OR(const TEPAPA_Result& r1, const TEPAPA_Result& r2) {
	MPL_retval_struct  retval;
	retval.bprof_out  = r1.bprof;
	retval.bprof_out |= r2.bprof;
	
	if ( retval.bprof_out == r1.bprof ) return retval;
	if ( retval.bprof_out == r2.bprof ) return retval;
		
	if ( retval.bprof_out.npos() <= 1 || retval.bprof_out.npos() >= retval.bprof_out.size() - 1 ) return retval;
	
	retval.mp = new meta_pattern( ght("OR") );
	retval.mp->push_back( r1.patt );
	retval.mp->push_back( r2.patt );
	
	return retval;
	}

MPL_retval_struct TEPAPA_Discoverer_MetaPattern::MPL_NOT(const TEPAPA_Result& r1) {
	MPL_retval_struct  retval;
	
	retval.bprof_out = ! r1.bprof;
	
	if ( retval.bprof_out == r1.bprof ) return retval;
		
	if ( retval.bprof_out.npos() <= 1 || retval.bprof_out.npos() >= retval.bprof_out.size() - 1 ) return retval;
	
	retval.mp = new meta_pattern( ght("NOT") );
	retval.mp->push_back( r1.patt );
	
	return retval;
	}




bool TEPAPA_Discoverer_MetaPattern::do_evaluate(const MPL_retval_struct& pat_match) {		
	
  if (! pat_match.mp) return false;
  pattern* p = pat_match.mp->clone();
  iptr<pattern> pp = p;

  evaluator -> eval_symbolic( pat_match.bprof_out, outvar_vectors_t(v_score), pp );
	
  return true;
}
	


#include "stats.h"

MPL_retval_struct mk_meta_pattern(hash_value hv_op, const TEPAPA_Results& rr, const vector<unsigned int>& v) {

  MPL_retval_struct  retval;
	
  retval.bprof_out = rr[v[0]].bprof;
  
  for (unsigned int i=1; i<v.size(); ++i) retval.bprof_out &= rr[v[i]].bprof;
  
  if ( retval.bprof_out.npos() <= 1 || retval.bprof_out.npos() >= retval.bprof_out.size() - 1 ) return retval;
  
  meta_pattern* mpp = new meta_pattern( hv_op ); // ght("AND")
  
  for (unsigned int i=0; i<v.size(); ++i) {
    mpp->push_back( rr[v[i]].patt->clone() );
  }
  
  retval.mp = mpp;
  
  return retval;
}


MPL_retval_struct mk_meta_pattern(hash_value hv_op, const TEPAPA_Results& rr, const vector<unsigned int>& v, const vector<bool>& v_neg) {

  MPL_retval_struct  retval;
	
  retval.bprof_out = rr[v[0]].bprof;
  
  for (unsigned int i=1; i<v.size(); ++i) retval.bprof_out &= rr[v[i]].bprof;
  
  if ( retval.bprof_out.npos() <= 1 || retval.bprof_out.npos() >= retval.bprof_out.size() - 1 ) return retval;
  
  meta_pattern* mpp = new meta_pattern( hv_op ); // ght("AND")
  
  for (unsigned int i=0; i<v.size(); ++i) {
    if ( v_neg[i] == true )  {
      meta_pattern* mpp1 = new meta_pattern( ght("NOT") );
      mpp1->push_back( rr[v[i]].patt->clone() );
      iptr<pattern> impp1 = mpp1;
      mpp->push_back( impp1 );
    }
    else {
      mpp->push_back( rr[v[i]].patt->clone() );
    }
  }
  
  retval.mp = mpp;
  
  return retval;
}






bool TEPAPA_Discoverer_MetaPattern::gen_MPL_AND_recursive(const TEPAPA_Results& rr, const vector<unsigned int>& v, const vector<bool>& neg, const binary_profile& bprof, unsigned int limit, unsigned int delta) {
  if (v.size() >= limit) return false;
  if (delta >= rr.size()) return false;

  vector<unsigned int> v1 = v;
  vector<bool> neg1 = neg;
  
  v1.push_back(delta);
  neg1.push_back(false);

  
  two_class_stats tcs;
  tcs.compute(v_score, bprof);

  //  tcs.print("efghabcdjJl"); printf(" ---- \n");
  
  for(unsigned int i=delta; i<rr.size(); ++i) {
    v1[ v1.size() - 1 ] = i;
    neg1[ v1.size() - 1 ] = false;

    // if new pattern is a complete subset of any of previous pattern, there is no point to add it in
    bool f_skip_intersection = false;
    for(unsigned int j=0; j<v.size(); ++j) {
      if ( rr[ v[j] ].bprof.contains(rr[i].bprof) ) {
	 f_skip_intersection = true;
	break;
      }
    }
   
    if (! f_skip_intersection) {
    
      binary_profile  bprof1 = bprof;
      bprof1 &= rr[i].bprof;
        
      two_class_stats tcs1;
      tcs1.compute(v_score, bprof1);    
      
      if ( tcs1.lrp() > tcs.lrp() ) { // v1 is a pattern with higher likelihood ratio than v
	/* tcs1.print("efghabcdjJl");  printf("\t"); for (unsigned int i=0; i<v.size(); ++i) printf(" %d", v[i]);  printf("\t%s", rr[i].patt->to_string().c_str()); */       
	if (v1.size() > 1) do_evaluate( mk_meta_pattern(ght("AND"), rr, v1, neg1) ); 
	
	if ( (v1.size() < limit) && (tcs1.fp>0) ) gen_MPL_AND_recursive(rr, v1, neg1, bprof1, limit, i+1);
      } 
    }

    /*
    // Try negation.

    if ( v1.size() > 1 ) {
      binary_profile  bprof1 = bprof;
      bprof1 &= ( ! rr[i].bprof );
      neg1[ v1.size() - 1 ] = true;
      tcs1.compute(v_score, bprof1);    
      
      if ( tcs1.lrp() > tcs.lrp() ) { // v1 is a pattern with higher likelihood ratio than v
	// tcs1.print("efghabcdjJl");  printf("\t"); for (unsigned int i=0; i<v.size(); ++i) printf(" %d", v[i]);  printf("\t%s", rr[i].patt->to_string().c_str()); 
	if (v1.size() > 1) do_evaluate( mk_meta_pattern(ght("AND"), rr, v1, neg1) ); 
	
	if ( (v1.size() < limit) && (tcs1.fp>0) ) gen_MPL_AND_recursive(rr, v1, neg1, bprof1, limit, i+1);
	} 
	}*/

  }
  return true;
}




bool TEPAPA_Discoverer_MetaPattern::gen_MPL_FOLLOW_recursive(const sample_list& sl, const TEPAPA_Results& rr, const vector<unsigned int>& v, const vector<size_t>& sli, const binary_profile& bprof, unsigned int limit) {
  if (v.size() >= limit) return false;

  vector<unsigned int> v1 = v;
  v1.push_back(0);

  two_class_stats tcs, tcs1;
  tcs.compute(v_score, bprof);
  
  for(unsigned int i=0; i<rr.size(); ++i) {
    v1[ v1.size() - 1 ] = i;
    
    binary_profile  bprof1 = bprof;
    bprof1 &= rr[i].bprof;
    
    tcs1.compute(v_score, bprof1);
    if ( tcs1.tp <= 1 ) continue;

    vector<size_t> sli1 = sli;
    assert(sl.size() == sli.size());

    for(unsigned int j=0; j<bprof1.size(); ++j) {
      if (! bprof1[j]) continue;
      
      pattern* ppi = rr[i].patt ;
      matchable_pattern* mp1 = dynamic_cast<matchable_pattern*>(ppi);
      if (! mp1) {
	bprof1[j] = false;
	continue;
      }

      matched_pos m = mp1->find( sl[j].data, matching_range(sl[j].data, sl[j].data.begin() + sli[j]) );
      
      if ( m.first == sl[j].data.end() ) bprof1[j] = false;
      
      sli1[j] = m.second - sl[j].data.begin();
    }

    tcs1.compute(v_score, bprof1);
    if ( tcs1.tp <= 1 ) continue;
    
    if ( tcs1.lrp() > tcs.lrp() ) { // v1 is a pattern with higher likelihood ratio than v
      /*  tcs1.print("efghabcdjJl");  printf("\t"); for (unsigned int i=0; i<v.size(); ++i) printf(" %d", v[i]);  printf("\t%s", rr[i].patt->to_string().c_str()); */       
      if (v1.size() > 1) do_evaluate( mk_meta_pattern(ght("FOLLOW"), rr, v1) );
      
      if ( (v1.size() < limit) && (tcs1.fp>0) ) gen_MPL_FOLLOW_recursive(sl, rr, v1, sli1, bprof1, limit);
    }
  }
  return true;
}





bool TEPAPA_Discoverer_MetaPattern::run1_meta(const sample_list& sl, const TEPAPA_Results& rr_input) {	
  TEPAPA_Results  rr = rr_input;

  binary_profile bprof( v_score.size() ) ;
  bprof = true;
  
  vector<unsigned int> v;
  vector<bool> v_neg;
  
  gen_MPL_AND_recursive(rr, v, v_neg, bprof, depth, 0);

  vector<size_t> sli( bprof.size() );  for(unsigned int i=0; i<sli.size(); ++i) sli[i]=0;
  
  //  gen_MPL_FOLLOW_recursive(sl, rr, v, sli, bprof, depth);

  
  return true;
}
