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

#include "tepapa-selsig.h"
#include "tepapa-utils.h"
#include "tepapa-msg.h"

#include <stdlib.h>


/////////////////////////////////////////////////

double get_alpha_benjamini (const TEPAPA_Results& rr, double fdr) {
	vector<double>  pvalues;
	
	for(unsigned int i=0; i<rr.size(); ++i) pvalues.push_back( rr[i].pval ) ;
	sort( pvalues.begin(), pvalues.end() );
	int bh_m = pvalues.size();
	int bh_i = 0;
	double alpha1_bh = 1.00;
	
	for(bh_i = 0; bh_i < bh_m; ++bh_i) {
// 		int j = int( pvalues.size() ) - bh_i + 1;
		alpha1_bh = double(bh_i + 1) / double(bh_m) * fdr;
		if ( pvalues[bh_i] >= alpha1_bh ) break;
		}
	
	return alpha1_bh;
	}


double get_alpha_ranked_top_n (const TEPAPA_Results& rr, unsigned int topn) {
	vector<double>  pvalues;
	
	for(unsigned int i=0; i<rr.size(); ++i) pvalues.push_back( rr[i].pval ) ;
	
	if (!pvalues.size()) return 1.00;
	
	sort( pvalues.begin(), pvalues.end() );
	
	if (topn < pvalues.size() - 1) return pvalues[topn+1];
	
	return 1.00;
	}



// Select by statistical significance


#define TEPAPA_SELSIG_SET_ALPHA       1
#define TEPAPA_SELSIG_SET_BONFERRONI  2
#define TEPAPA_SELSIG_SET_BENJAMINI   3
#define TEPAPA_SELSIG_SET_RANKED      4


TEPAPA_Program_SigSelect::TEPAPA_Program_SigSelect(): TEPAPA_Program("@SigSelect") {
	sel_method = 0;
	alpha0 = 0.05;
	fdr = 0.20;
	topn = 0;
	
	// Specified alpha
	options_optarg.push_back( 
		TEPAPA_option_optarg ("-a", "--alpha", "", 'f', &alpha0, 
				[&](const string& optarg) -> bool { 
				sel_method = TEPAPA_SELSIG_SET_ALPHA;  
				return true;
				}
			)
		); 
	
	options_optarg.push_back( 
		TEPAPA_option_optarg ("-b", "--alpha-bonferroni", "", 'f', &alpha0, 
				[&](const string& optarg) -> bool { 
				sel_method = TEPAPA_SELSIG_SET_BONFERRONI; 
				return true;
				}
			)
		); 
	
	options_optarg.push_back( 
		TEPAPA_option_optarg ("-f", "--fdr", "", 'f', &fdr,
				[&](const string& optarg) -> bool { 
				sel_method = TEPAPA_SELSIG_SET_BENJAMINI;  
				return true;
				}
			)
		); 
	
	options_optarg.push_back( 
		TEPAPA_option_optarg ("-r", "--top-n", "", 'i', &topn, 
				[&](const string& optarg) -> bool { 
				sel_method = TEPAPA_SELSIG_SET_RANKED;     
				return true;
				}
			)
		); 
	}

	
	
int TEPAPA_Program_SigSelect::run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) {

	map<hash_value, double>  case_summary;

	TEPAPA_Results&  rr = ds_input.rr;
	
	ds_input.sl.summarise( case_summary ); 
	
	int n_ubprof = rr.get_unique_binary_profiles();
	
	msgf(VL_INFO, "Unique patterns         = %lu\n", rr.size());
	msgf(VL_INFO, "Unique binary profiles  = %d\n", n_ubprof );
	msgf(VL_INFO, "Cases                   = %d\n", (int) case_summary[ ght("Cases") ] );
	msgf(VL_INFO, "Unique primary tokens   = %d\n", (int) case_summary[ ght("Unique primary tokens") ] );
	msgf(VL_INFO, "Unique auxillary tokens = %d\n", (int) case_summary[ ght("Unique auxillary tokens") ] );
	msgf(VL_INFO, "Unique tokens           = %d\n", (int) case_summary[ ght("Unique tokens") ] );
	msgf(VL_INFO, "Total length            = %d\n", (int) case_summary[ ght("Total length") ] );
	msgf(VL_INFO, "Avg length per case     = %f\n", case_summary[ ght("Avg length per case") ] );
	
	double estimated_N = n_ubprof; // ( case_summary[ ght("Unique primary tokens") ] );
		
	double alpha1_bonefrroni = alpha0 / estimated_N ; 
	double alpha1_bh = get_alpha_benjamini(rr, fdr);
	double alpha1_ranked = get_alpha_ranked_top_n(rr, topn);
	
	if ( !sel_method ) msgf(VL_FATAL, "No significance selection methods specified");
	
	double alpha1 = 1.00;
	switch(sel_method) {
		case TEPAPA_SELSIG_SET_ALPHA:
			msgf(VL_INFO, "alpha0                  = %g\n", alpha0);
			alpha1 = alpha0; 
			break;
		case TEPAPA_SELSIG_SET_BONFERRONI: 
			msgf(VL_INFO, "alpha1 (Bonferroni)     = %g\n", alpha1_bonefrroni );
			alpha1 = alpha1_bonefrroni; 
			break;
		case TEPAPA_SELSIG_SET_BENJAMINI: 
			msgf(VL_INFO, "alpha1 (Benjamini)      = %g\n", alpha1_bh);
			alpha1 = alpha1_bh; 
			break;
		case TEPAPA_SELSIG_SET_RANKED: 
			msgf(VL_INFO, "alpha1 (Ranked, top %d) = %g\n", topn, alpha1_ranked);
			alpha1 = alpha1_ranked; 
			break;
		default: ;
		};

	TEPAPA_Results& rr0 = rr;
	
	TEPAPA_Results res;
	
	
	if ( sel_method == TEPAPA_SELSIG_SET_RANKED ) {
		res = rr0;
		sort ( res.begin(), res.end(), [](const TEPAPA_Result& a, const TEPAPA_Result& b) -> bool { return a.pval < b.pval; } );
		if ( (unsigned int)topn > res.size() ) topn = res.size();
		res.resize( topn ); 
		}
	else {
		res.reserve( rr0.size() / 4 );
		for(unsigned int i=0; i<rr.size(); ++i) {
			if ( rr[i].pval < alpha1 ) {
				res.push_back( rr[i] );
				}
			}
		}

	msgf(VL_INFO, "After selection = %lu\n", res.size() );

	rr_output = res;
	
	return TEPAPA_RETVAL_SUCCESS;
	}

