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

#include "tepapa-dataset.h"
#include "tepapa-msg.h"
#include "strfriends.h"

bool TEPAPA_dataset::save_csv(
	const string& p_output_filename,
	bool f_gen_arff_headers,
	bool f_do_csv_binary_outcome,
	bool f_write_case_id,
	bool f_numeric_attribute,
	const char* delimiter,
	const char* quote,
	const char* missing
	) {
	FILE* fout = stdout;
	
	if ( strlen( p_output_filename.c_str() ) > 0 ) {
		if ( ! (fout = fopen(p_output_filename.c_str(), "wt") ) ) {
			perror("");
			msgf(VL_FATAL, "Unable to write to file");
			}
		}
	
	if (f_gen_arff_headers) {
		fprintf( fout, "@relation TEPAPA-dataset\n" );
		}
	
	if (f_write_case_id) {
		if (f_gen_arff_headers) { // WEKA ARFF
			fprintf( fout, "@attribute %sCase number%s real\n", quote, quote);
			fprintf( fout, "@attribute %sCase name%s   string\n", quote, quote);
			}
		else { // normal CSV
			fprintf( fout, "%sCase number%s%s", quote, quote, delimiter);
			fprintf( fout, "%sCase name%s%s", quote, quote, delimiter);
			}
		}
	
	for (unsigned int j=0; j<rr.size(); ++j) {
		pattern* rrjp = rr[j].patt;
		string t = ght[ rrjp ->get_type_id() ];
		string s = escape_str( rrjp ->to_string() );
		bool f_is_numeric = ( rrjp ->get_type_id() == ght("EXNGRAM") );
		
		if (f_gen_arff_headers) { // WEKA ARFF
			fprintf( fout, "@attribute %s%s:%s%s  %s\n", quote, t.c_str(), s.c_str(), quote, ( (f_is_numeric || f_numeric_attribute) ? "real" : "{'Y', 'N'}") );
			}
		else {
			fprintf( fout, "%s%s:%s%s%s", quote, t.c_str(), s.c_str(), quote, delimiter);
			}
		}
	
	if (f_gen_arff_headers) { // WEKA ARFF
		fprintf( fout, "@attribute %s%s%s  %s\n", 
				 quote, (f_do_csv_binary_outcome ? "Class" : "Score"), quote, 
				 (f_do_csv_binary_outcome ? "{'Y', 'N'}" : "real") 
			   );
		fprintf( fout, "\n@data\n");
		}
	else {
		fprintf( fout, "%s%s%s", quote, (f_do_csv_binary_outcome ? "Class" : "Score"), quote);
		fprintf( fout, "\n");
		}
	
// 	printf( "!%s\t%u\t%u\n", active_dataset_name.c_str(), sl.size(), rr.size());
	
	for (unsigned int i=0; i<sl.size(); ++i) {
		vector<hash_value>&  def = sl[i].definitions;
		
		if (f_write_case_id) {
			fprintf( fout, "%0d%s", i+1, delimiter );
			if ( def.size() > 0)  {
				hash_value hv = def[0];
				fprintf( fout, "%s%s%s%s", quote, ght[ hv ], quote, delimiter ); 
				}
			else {
				fprintf( fout, "%sCASE%05d%s%s", quote, i+1, quote, delimiter );
				}
			}
		
		for (unsigned int j=0; j<rr.size(); ++j) {
			string s = rr[j].patt->to_string();
			binary_profile& bp = rr[j].bprof;
			bool flag = bp[i];
			bool f_is_numeric = ( rr[j].patt->get_type_id() == ght("EXNGRAM") );
			
			if ( f_is_numeric ) {
				fprintf( fout, "%s%s", (flag ? strfmt("%g", rr[j].val[i]).c_str() : missing), delimiter);
				}
			else {
				if (f_numeric_attribute) {
					fprintf( fout, "%s%s", (flag ? "1" : "0"), delimiter);
					}
				else {
					fprintf( fout, "%s%s", (flag ? "Y" : "N"), delimiter);
					}	
				}
			}
		
		if (f_do_csv_binary_outcome) {
			fprintf( fout, "%s", ( (sl[i].score!=0.0) ? "Y" : "N") );
			}
		else {
			fprintf( fout, "%g", sl[i].score);
			}
		fprintf( fout, "\n");
		}
	
	if (fout != stdout) {
		fclose(fout);
		}
	
	return true;
	}


void feature_array::load_from(const TEPAPA_dataset& ds, int sl_i) {
	size_t  dim = ds.rr.size();
	
	resize(dim) ;
	
	for(unsigned int j=0; j<dim; ++j) {
		bool f_is_numeric = ds.rr[j].is_numeric();
			
		mask_numeric[j] = f_is_numeric ;
		bp[j] = ds.rr[j].bprof[sl_i];
		if ( f_is_numeric ) {
			vp[j] = ds.rr[j].val[sl_i];
			}
		else {
			vp[j] = ds.rr[j].bprof[sl_i] ? 1.0 : 0.0;
			}
		}
		
	score = ds.sl[ sl_i ].score;
	name_str = ght[ ds.sl[ sl_i ].definitions[0] ];
	}



void feature_arrays::load_from(const TEPAPA_dataset& ds) {
	resize( ds.sl.size() );
	
	for(unsigned int j=0; j<ds.rr.size(); ++j) {
		iptr<pattern> pp = ds.rr[j].patt;
// 		printf( "-- %s\n", pp->to_string().c_str() );
		patterns.push_back( pp );
		}

	for(unsigned int i=0; i<ds.sl.size(); ++i) {
		iterator it = begin() + i;
		it -> load_from(ds, i);
		}
	}

bool feature_arrays::has_numeric_features() const {
	for(unsigned int i=0; i<patterns.size(); ++i) {
		if ( patterns[i]->is_numeric() ) return true;
		}
	return false;
	}
