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
#include "tepapa-sumpatt.h"

TEPAPA_Program_Pattern_Summariser::TEPAPA_Program_Pattern_Summariser()
	: TEPAPA_Program("@SummarisePattern") {
		
	n_iters_base     = 0     ; 
	f_cut            = 0.0   ;
	f_export_markers = false ;
	
	options_optarg.push_back( 
		TEPAPA_option_optarg ("-n", "--num-trials", "", 'i', &n_iters_base)
		);
	
	options_optarg.push_back( 
		TEPAPA_option_optarg ("-c", "--cut-threshold", "", 'f', &f_cut)
		);

	options_binary.push_back( 
		TEPAPA_option_binary ("-m", "--export-as-marker", "", 'b', &f_export_markers)
		);
	}

bool TEPAPA_Program_Pattern_Summariser::handle_argv(const vector<string>& argv) {
	ds_comparison = argv;
	return true;
	}

	
template<typename TK, typename TV> 
	std::vector<TK> extract_keys(std::map<TK, TV> const& input_map) {
		std::vector<TK> retval;
		for (auto const& element : input_map) {
			retval.push_back(element.first);
			}
		return retval;
	}

	
int group_by_pattern(map<hash_value,TEPAPA_Results>& out, const TEPAPA_Results&  rr_in) {
	out.clear();
	
	for(unsigned int i=0; i<rr_in.size(); ++i) {
		hash_value hv_uniq_id_patt = rr_in[i].patt->get_uniq_id();
		out[ hv_uniq_id_patt ].push_back( rr_in[i] );
		}
	
	return 1;
	}

#include "strfriends.h"

int TEPAPA_Program_Pattern_Summariser::run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) {
	map<hash_value,TEPAPA_Results>  hrr1;
	vector< map<hash_value,TEPAPA_Results> > hrr2_list;
	
	group_by_pattern(hrr1, ds_input.rr);

	vector<hash_value> hrr1s = extract_keys(hrr1);
	vector<int> rediscovery_cnt( ds_comparison.size() ) ;
		
	sort( hrr1s.begin(), hrr1s.end(), [&](const hash_value& a, const hash_value& b) -> bool { 
			return hrr1[a].size() > hrr1[b].size(); 
			} 
		);

	TEPAPA_global_data_struct& gvr = *gvr_ptr;
	TEPAPA_Results out;


	for(unsigned int i=0; i<ds_comparison.size(); ++i) {
		TEPAPA_dataset&  cmp_dataset = gvr.get_dataset( ds_comparison[i] );
		TEPAPA_Results&  cmp_rr = cmp_dataset.rr;
		map<hash_value,TEPAPA_Results>  hrr2;
		
// 		printf("!");
// 		cmp_rr.dump();
// 		printf("!");
		
		group_by_pattern(hrr2, cmp_rr) ;
		hrr2_list.push_back( hrr2 ) ;
		}
	
	for(unsigned int i=0; i<hrr1s.size(); ++i) {
		hash_value hv = hrr1s[i];
		int cnt1 = hrr1[ hv ].size();
		
		string result_line ;
		
		if (n_iters_base) {
			double p1 = double(cnt1) / double(n_iters_base) ;
			if ( p1 < f_cut ) continue;
			result_line  += strfmt("%d\t(%.1f%%)\t", cnt1, p1 * 100 );
			}
		else {
			if ( cnt1 < f_cut ) continue;
			result_line  += strfmt("%d\t", cnt1 );
			}

		unsigned int nmatch = 0;
		for(unsigned int j=0; j<ds_comparison.size(); ++j) {
			map<hash_value,TEPAPA_Results>&  hrr2 = hrr2_list[j];
// 			if ( hrr2.find(hv) == hrr2.end() )  continue;
			int cnt2 = hrr2[ hv ].size();
			if (n_iters_base) {
				double p2 = double(cnt2) / double(n_iters_base)  ;
				result_line  += strfmt("%d\t(%.1f%%)\t", cnt2, p2 * 100 );
				}
			else {
				result_line  += strfmt("%d\t", cnt2 );
				}
			if (cnt2) {
				rediscovery_cnt[j] ++;
				nmatch ++;
				}
			}

// 		if ( ds_comparison.size() && (! nmatch) ) continue;
		if ( (! nmatch) ) continue;
		
		iptr<pattern> pp0 = hrr1[ hv ][0].patt;
		
		result_line  += strfmt("%s\t%s", ght[ pp0->get_type_id() ], pp0->to_string().c_str() );
		printf("%s\n", result_line.c_str());
	
		TEPAPA_Result  r;
		
		r.patt = pp0;
		
		out.push_back(r);
		}

	printf("Rediscovery rates:\n");
	for(unsigned int j=0; j<ds_comparison.size(); ++j) { 
		printf("%s\t%u\t%u\t%f\n", ds_comparison[j].c_str(), hrr1.size(), rediscovery_cnt[j], double(rediscovery_cnt[j] ) / double(hrr1.size()) );
		}
	
	if ( f_export_markers) {
		rr_output = out;
		return TEPAPA_RETVAL_SUCCESS ;
		}
	else {
		return TEPAPA_RETVAL_SUCCESS | TEPAPA_RETVAL_TERMINUS;
		}
	}
