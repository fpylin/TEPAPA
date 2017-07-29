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

#include "tepapa-numeric.h"


int TEPAPA_Discoverer_Numeric::traverse_ngram_classes_spearman(vector<extractable_ngram_pattern>& out_rpl, hash_value index_class, token_string::const_iterator index, int ka, int kb) {
	vector<int> kl;
	vector<int> nelem;
	vector<int> multiplier;
	
	int n = 1;
	for(int k=ka; k<=kb; ++k) {
		kl.push_back(k);
		if (k == 0)
			nelem.push_back(1);
		else {
			int nz = (index+k)->size();
			n *= nz;
			nelem.push_back(nz);
			}
		}
	
	int dim = nelem.size();
	multiplier.resize( dim ) ;
	
	for(int i=dim-1; i>=0; --i) {
		if(i==dim-1) {
			multiplier[i] = 1;
			}
		else {
			multiplier[i] = multiplier[i+1] * nelem[i+1] ;
			}
// 		printf("%d\t%d\t%d\n", kl[i], nelem[i], multiplier[i]);
		}
// 	printf("%d\n", n);
	
	vector<int> digits(dim);
	for(int num=0; num<n; ++num) {
		int r = num;
		
		int sum_digits = 0;
		for(int d=0; d<dim; ++d) {
			int m = multiplier[d];
			int z = r / m ;
			digits[d] = z;
			r -= z * m;
			
			sum_digits += z;
			}
		
		if (!f_deep_cmp && sum_digits) continue; // use primary only;
		
// 		printf ("# %d\t", num);
// 		for(int di=0; di<dim; ++di) printf (" %d", digits[di]);
		extractable_ngram_pattern erp(dim);
		for(int j=0; j<dim; ++j) {
			if ( kl[j] == 0 ) {
				erp[j] = index_class;
				erp.tag_i = j;
				}
			else {
				erp[j] = (*(index+kl[j]))[digits[j]];
				}
			}
		out_rpl.push_back(erp);
// 		printf("\n");
		}
// 	printf("\n");
	return out_rpl.size();
	}




void TEPAPA_Discoverer_Numeric::run1(int i) {
	
	TEPAPA_Evaluator& e = *evaluator;
	
	sample_list& sl = *sl_ptr;
	
	const token_string& s = sl[i].data;

	for (token_string::const_iterator j=s.begin(); j!=s.end(); ++j) {
		if ( ! j->size() ) continue;

		if ( hv_separators.find( j->primary() ) != hv_separators.end() ) continue;
		
		if ( ! j->has_class(hv_class_NUMBER) ) continue;
		
		int ka_min = -4;
		int kb_max =  4;

		for (int ka=0; ka>=ka_min; ka--) {
			if ( (j + ka) < s.begin()) break;
			if ( hv_separators.find( (j + ka)->primary() ) != hv_separators.end() ) break;

			for (int kb=0; kb<=kb_max; kb++) {
				if ( (j + kb) >= s.end()) break;
				if ( hv_separators.find( (j + kb)->primary() ) != hv_separators.end() ) {
					kb_max = kb - 1;
					break;
					}

				vector<extractable_ngram_pattern> rpl;
				
				traverse_ngram_classes_spearman(rpl, hv_class_NUMBER, j, ka, kb);

				for(unsigned int z=0; z<rpl.size(); ++z) {
					string s = rpl[z].to_string();
					
					hash_value h = ght( s.c_str() );
					
					if ( hr.exists(h) ) continue;
					
					hr.set_existence(h);
					
					binary_profile bp_mask;
					
					vector<double> values = rpl[z].eval(sl, bp_mask);
					
					iptr<pattern> pp = new ngram_pattern( rpl[z] );
					
					if ( e.eval_numeric(bp_mask, v_score, values, pp) ) total_cnt_cls ++;
					}
				}
			}
		}
	}


