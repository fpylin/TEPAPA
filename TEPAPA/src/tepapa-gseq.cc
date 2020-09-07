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
#include "tepapa-gseq.h"
#include "tepapa-symbolic.h"


TEPAPA_Program_Discoverer_Gapped_Sequence::TEPAPA_Program_Discoverer_Gapped_Sequence()
  : TEPAPA_Program_Discoverer<TEPAPA_Gapped_Sequence_Discoverer>("@DiscoverGappedSequence") {
  param[TEPAPA_ARGSTR_DISCOVERER_MAX_NGRAM] = 100;
	
  options_binary.push_back(
   TEPAPA_option_binary(TEPAPA_ARGSTR_DISCOVERER_USE_WILDCARD, "--use-wildcard", "", 'b')
			   );
	
  options_binary.push_back(
   TEPAPA_option_binary(TEPAPA_ARGSTR_DISCOVERER_USE_DEEP_CMP, "--use-deep-comparison", "", 'b') 
			   );
  
  options_binary.push_back( 
   TEPAPA_option_binary(TEPAPA_ARGSTR_DISCOVERER_POSITIVE_ONLY, "--positive-pattern-only", "", 'b')
			    ); 
  
  options_optarg.push_back( 
   TEPAPA_option_optarg(TEPAPA_ARGSTR_DISCOVERER_MAX_NGRAM, "--max-ngram", "", 'i')
			    ); 
  

}


#include "fgroup.h"

// run_recursive



int TEPAPA_Gapped_Sequence_Discoverer::traverse_gseq_btree
(TEPAPA_Evaluator& e,
 const gapped_sequence_pattern& gs,
 size_t index,
 const binary_profile& mmask,
 const matching_range_list& mrlist,
 int depth,
 int wanted_dir
 ) {
  int cnt = 0;
  const sample_list& sl = *sl_ptr;
  
  assert( sl.size() == mrlist.size() );
  assert( sl.size() == mmask.size() );

  TEPAPA_Discoverer_Symbolic*  discoverer = new TEPAPA_Discoverer_Symbolic(*p_param);
  
  if ( ! discoverer ) {
    msgf(VL_FATAL, "Unable to create discoverer");
    return false;
  }
  
  discoverer -> set_sample_list( sl );
  discoverer -> set_substr_mask( mrlist ); 
  discoverer -> set_init_search_mask( mmask );   
  discoverer -> run();
  
  TEPAPA_Results rr = discoverer->get_results();

  rr = rr.filter('p', TEPAPA_RESULT_FILTER_LT, 0.05);
  
  rr.label_by_signature("b");
  
  map<int, TEPAPA_Results>  m = rr.group_by_signature<int>();

  TEPAPA_Results rr1;
  
  reduce_by_collapse_subpatterns(rr1, m, sl) ;

  rr = rr1;
  
  rr.sort("dpen");
  
  rr.dump("gdmepntx");
  
  delete discoverer;
  
  for (size_t i=0; i<rr.size(); ++i) {
    //    if (i==50) break;
    gapped_sequence_pattern gs_new = gs;
     
    //    printf("[%s]\n", rr[i].patt->to_string("  ").c_str());
    if ( (wanted_dir > 0) && (rr[i].dir<=0) ) continue;
    if ( (wanted_dir < 0) && (rr[i].dir>=0) ) continue;
  
    pattern* pp = rr[i].patt;
    ngram_pattern* mpp = dynamic_cast<ngram_pattern*>(pp);
    if (! mpp) continue;

    gs_new.insert( (gs_new.begin() + index), *mpp );

    iptr<pattern> pp_gsnew = new gapped_sequence_pattern( gs_new );

    binary_profile vb = gs_new.match(sl);
    
    e.eval_symbolic( vb, outvar_vectors_t(v_score, v_censored), pp_gsnew );
    cnt ++;

  
    binary_profile        mmask_new;
    binary_profile        mmask_front;
    binary_profile        mmask_back;
    matching_range_list   mrlist_front;
    matching_range_list   mrlist_back;

    binary_profile::const_iterator mm_i = mmask.begin();
    matching_range_list::const_iterator mr_i = mrlist.begin();
    
    for(sample_list::const_iterator i=sl.begin(); i!=sl.end(); ++i, ++mr_i, ++mm_i) {

      const token_string& ts = i->data;

      token_string::const_iterator z;

      if ( (*mm_i) ) {
	z = mpp->find(ts, matching_range(ts)).first;
      }

      if ( (! (*mm_i)) || (z == ts.end() ) ) {
	mmask_new.push_back(false);
	mmask_front.push_back(false);
	mmask_back.push_back(false);
	mrlist_front.push_back( *mr_i );
	mrlist_back.push_back( *mr_i );
	continue;
      }
      
      mmask_new.push_back(true);


      token_string::const_iterator ss0 = ts.begin() + mr_i->from;
      token_string::const_iterator ss = ts.begin();
      token_string::const_iterator z0 = z ;

      if ( z0 != ts.begin() ) {      
	for (ss = z0-1; (ss>ts.begin() && ss>ss0); ss--) {
	  if ( hv_separators.find( ss->primary() ) != hv_separators.end() ) break;
      	}
      }
      
      // ss = ss0;
      

      token_string::const_iterator se0 = ts.begin() + mr_i->to;
      token_string::const_iterator se = ts.end();
      token_string::const_iterator z1 = z + mpp->size();      

      if ( z1 < ts.end() ) {
	for (se = z1+1; (se<ts.end()) && (se<se0); se++) {
	  if ( hv_separators.find( se->primary() ) != hv_separators.end() ) break;
	}
      }
      
      // se = se0;

      while (z != ts.end() ) { // find the last z
	z0 = z;
	z = mpp->find(ts, matching_range(ts, z0+1) ).first;
      }
      

      if ( (z0 > ts.begin()) && (ss < z0-1) ) {
	mrlist_front.push_back( matching_range( ts, ss, z0 - 1)  );
	mmask_front.push_back(true);
      }
      else {
	mrlist_front.push_back( matching_range( ts, z0, z0 )  );
	mmask_front.push_back(false);
      }

      

      if ( ( z1 < ts.end() ) && (z1 < se) ) {
	mrlist_back.push_back( matching_range( ts, z1, se) );
	mmask_back.push_back(true);
      }
      else {
	mrlist_back.push_back( matching_range( ts, z1, z1)  );
	mmask_back.push_back(false);
      }
      
      /*            printf("%d\t%d\tfront\t%d\t%d\tback\t%d\t%d\n",
	     (i-sl.begin()), (z - ts.begin()),
	     (ss-ts.begin()), (z-1-ts.begin()),
	     (z+mpp->size()-ts.begin()), (se-ts.begin())
	     );*/
    
    }  
    //    if ( mmask_new.npos() < 3) continue;
    printf("MMASK: %d\t%s\t%s\n",
	   mmask_new.npos(),
	   "", // mmask_new.to_string().c_str(),
	   gs_new.to_string(" ").c_str()
	   );
    if (depth > 0) {
      //      cnt += traverse_gseq_btree(e, gs_new, index ,    mmask_front, mrlist_front, depth-1, -rr[i].dir);
      //      cnt += traverse_gseq_btree(e, gs_new, index + 1, mmask_back,  mrlist_back,  depth-1, -rr[i].dir);
      cnt += traverse_gseq_btree(e, gs_new, index ,    mmask_front, mrlist_front, depth-1, 0);
      cnt += traverse_gseq_btree(e, gs_new, index + 1, mmask_back,  mrlist_back,  depth-1, 0);
    }
  }
  return cnt;
}						  




bool TEPAPA_Gapped_Sequence_Discoverer::run() {
  //  TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output
  const sample_list& sl = *sl_ptr;
  
  evaluator= new TEPAPA_Evaluator( sl);

  stopwatch  sw;

  sw.start();

  initialise();

  //  VariableList& param = *(p_param);
  //  bool f_positive_only = int( param[TEPAPA_ARGSTR_DISCOVERER_POSITIVE_ONLY] );
  
  binary_profile        mmask( sl.size() );  mmask=true;
  matching_range_list   mrlist;
  for(auto i=sl.begin(); i!=sl.end(); ++i) mrlist.push_back( matching_range(i->data) );
  
  gapped_sequence_pattern gs;

  //  msgf(VL_NOTICE, "Total = %lu results\n", total_cnt_cls);
  
  traverse_gseq_btree(*evaluator, gs, 0, mmask, mrlist, 3);
  
  return TEPAPA_RETVAL_SUCCESS;  
}
