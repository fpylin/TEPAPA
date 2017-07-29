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

#include "tepapa-tag.h"
#include "tepapa-msg.h"
#include "utils.h"
#include "eperl.h"
#include "strfriends.h"

TEPAPA_Program_TagFile::TEPAPA_Program_TagFile(): TEPAPA_Program("@TagFile") {
	options_optarg.push_back( 
		TEPAPA_option_optarg ("-i", "--input-file", "", 's', &path_case_raw)
		); 
	
	options_optarg.push_back( 
		TEPAPA_option_optarg ("-o", "--output-file", "", 's', &path_output)
		); 
	}


bool TEPAPA_Program_TagFile::handle_argv(const vector<string>& argv) {
	
// 	TEPAPA_global_data_struct& gvr = *gvr_ptr;
	
	if (! argv.size() && ! path_case_raw.length() ) msgf(VL_FATAL, "No files to tag!");
	
	if ( path_case_raw.length() ) {
		sample_list  sl_in;
		sl_in.load( path_case_raw.c_str() );
		sl_in.sort();
		
		for(unsigned int i=0; i<sl_in.size(); ++i) {
			hash_value hv = sl_in[i].definitions[0];
			file_list.push_back( ght[ hv ] );
			score_list.push_back( sl_in[i].score );
			}
		}
	else {
		file_list = argv;
		for(unsigned int i=0; i<argv.size(); ++i) score_list.push_back(0);
		}
	
	return true;
	}

#include "ngram-pattern.h"

int TEPAPA_Program_TagFile::run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) {
	
	EmbeddedPerl  eperl(true);
	
	string output = "<link rel=\"stylesheet\" type=\"text/css\" href=\"tepapa-output.css\" />\n";
	
	for(unsigned int z=0; z<file_list.size(); ++z) {
		msgf(VL_INFO, "Tagging file [%s]\n", file_list[z].c_str() );
		
		string output1 ;
		output1 += string("<div class=case>\n");
		output1 += strfmt("<h1>FILE: %s (Score: %f) </h1>\n", file_list[z].c_str(), score_list[z] );
		
		string data = load_file(file_list[z]);
		
		vector< pair<string,string> >  prp; 
		prp.push_back( pair<string,string>("<", "&lt;") ); 
		prp.push_back( pair<string,string>(">", "&gt;") ); 
		data = eperl.simple_replacement(data, prp);
		
		TEPAPA_Results&  rr = ds_input.rr;
		
// 		printf( "\e[1;36m<h1>FILE: %s</h1>\e[0m\n", file_list[z].c_str() );
		
		for(unsigned int i=0; i<rr.size(); ++i) {
			string pcre_patt = rr[i].patt->gen_pcre();
			if ( ! pcre_patt.size() ) continue;
			
			string statement = string("my $data = <<'ENDDATA';\n");
			string tag_o = strfmt("<span class='%s lp%d%s' title='%s, %s=%g, P=%g'>", 
				(rr[i].dir > 0 ? "pos": "neg"), 
				int( -log(rr[i].pval) / log(10) ),
				(rr[i].sel ? " im" : ""),
// 				rr[i].patt->to_string(),
				(rr[i].dir > 0 ? "POS": "NEG"), 
				ght[ rr[i].method ],
				rr[i].est,
				(rr[i].pval)
				) ;
			string tag_c = string ("<\\/span>") + strfmt("<sub class=method>[%s]<\\/sub>", ght[ rr[i].patt->get_type_id() ]);
// 			string tag_o = strfmt("\\e[1;%dm<span class='%s lp%d'>", (rr[i].dir > 0 ? 32 : 31), (rr[i].dir > 0 ? "pos": "neg"), int( -log(rr[i].pval) ) ) ;
// 			string tag_c = string ("<\\/span>\\e[0m") ;
			
			statement += data;
			statement += string("ENDDATA\n\n");
			statement += strfmt("$regex = <<'ENDREGEX';\n%s\nENDREGEX\nchomp $regex;\n", pcre_patt.c_str() );
			statement += string("if( $regex =~ s/\\\\\\(UMLS:(.+?)\\\\\\)/$1/g ) { $regex =~ s/\\s+/\\s+/sg; }\n");
// 			statement += string("$regex =~ s/\\+/\\\\+/sg;\n");
// 			statement += string("$regex =~ s/(\\\\s[\\+\\*])/(?:\\\\s\\*|<\\/?span.*?>|<sub.+?>\\\\[\\w+\\\\]<\\/sub>)+/sg;\n");
// 			statement += string("$regex =~ s/(\\\\s[\\+\\*])/(?:\\\\\\s\\*|<\\/span>|<span [A-Za-z0-9=\\\\',\\\\.\\\\+\\\\-\\\"]+>)+/sg;\n");
// 			statement += string("print $regex.\"\\n\";\n");
// 			statement += strfmt("$regex =~ s/( +|\\\\s+)/\\s+/g;\n");
// 			if ( rr[i].patt->get_type_id() == ngram_pattern::type_id() ) {
// 				statement += strfmt("$regex =~ s/[\\(\\)\\[\\]\\?\\*\\.]/$1/g;\n");
// 				}
			statement += strfmt("$data =~ s/(?:\\b|^)($regex)(?:\\b|$)/", pcre_patt.c_str()) + tag_o + string("$1") + tag_c + string("/sg;\n");
			statement += strfmt("return $data;\n");
			
// 			printf("%s", statement.c_str());
// 			
			data = eperl.eval(statement);
			}
		data = eperl.simple_replacement(data, "\\n\\n+", "<p\\/>\\n");
		data = eperl.simple_replacement(data, "\\n", "<br\\/>\\n");
		data = eperl.simple_replacement(data, "<p\\/>\\s*(?:<br\\/>\\n)+", "<p\\/>\\n" );
		output1 += data;
		output1 += string("</div>\n");
		output += output1;	
		}

	
	if ( path_output.length() ) {
		save_file(path_output, output);
		}
	else {
		printf( "%s\n", output.c_str() );
		}

	return TEPAPA_RETVAL_SUCCESS | TEPAPA_RETVAL_TERMINUS;
	}
