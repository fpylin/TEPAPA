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

#include "predicate-pattern.h"

hash_value  predicate_pattern::predicate_pattern_hv_type_id = 0; 

map<hash_value, double(*)(const ngram_pattern&, const token_string&)>  predicate_pattern::func_table;

double func_NTF(const ngram_pattern& np, const token_string& ts) {
  vector<size_t> pos = np.find_all(ts);
  return double( pos.size() ) / double( ts.size() ); 
}

double func_TF(const ngram_pattern& np, const token_string& ts) {
  vector<size_t> pos = np.find_all(ts);
  return pos.size(); 
}

void predicate_pattern::initialise() {
  func_table[ ght("NTF") ] = &func_NTF;
  func_table[ ght("TF") ] = &func_TF;
}


double predicate_pattern::eval(const token_string&  ts) const {
  map<hash_value, double(*)(const ngram_pattern& np, const token_string&)>::const_iterator  z = func_table.find( hv_func ) ;
  if ( z == func_table.end() ) {
    return nan("");
  }
  return z->second(*this, ts);
}


	
TEPAPA_Program_Predicate_Discoverer::TEPAPA_Program_Predicate_Discoverer()
  : TEPAPA_Program("@DiscoverPredicate") {
  options_optarg.push_back( 
   TEPAPA_option_optarg ("-f", "--function", "", 's', &func_name)
    ); 
}

#include "tepapa-evaluator.h"

int TEPAPA_Program_Predicate_Discoverer::run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) {
	
  predicate_pattern::initialise() ;
	
  TEPAPA_Results& rr = ds_input.rr;
  
  TEPAPA_Results out;
  
  TEPAPA_Evaluator e( ds_input.sl );
  
  vector<double>  v_score = ds_input.sl.get_scores();
  
  hash_value  hv_func_name  = ght( func_name.c_str() );
  
  for(unsigned int i=0; i<rr.size(); ++i) {
    
    pattern* pp = rr[i].patt ;
    
    ngram_pattern* ppp = dynamic_cast<ngram_pattern*>( pp );
    
    predicate_pattern*  ntfp = new predicate_pattern( *ppp, hv_func_name );
    
    assert(ntfp);
    
    binary_profile bp_mask;
    
    bp_mask = true;
    
    vector<double> v_predictors = ntfp->eval( ds_input.sl, bp_mask) ;
		
// 		for(int j=0; j<v_predictors.size(); ++j) printf("%f ", v_predictors[j]);
    
    iptr<pattern>  new_pp = ntfp;
    
    e.eval_numeric(bp_mask, v_score, v_predictors, new_pp);
  }
  
  out = e.get_results();
  
  rr_output = out;
  
  return TEPAPA_RETVAL_SUCCESS;
}

