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

#include "pattern.h"
#include "tepapa-msg.h"

hash_value pattern::get_uniq_id() { 
	if ( ! hv_uniq_id ) {
		if (get_type_id() == 0) msgf(VL_FATAL, "hv_type_id not defined!");
		string uniq_str = string( ght[ get_type_id() ] ) + string(":") + to_string();
		hv_uniq_id = ght.hash( uniq_str );
		}
	return hv_uniq_id ; 
	}

/*
#include "strfriends.h"
	
string pattern::serialise() const { 
	string type_str =  ght[ get_type_id() ];
	string tag_o = strfmt("<%s>", type_str );
	string tag_c = strfmt("<%s/>", type_str );
	return tag_o + serialise_proper() + tag_c ;
	}

#include "ngram-pattern.h"

template <typename T> pattern* try_unserialise_pattern(const string& str) {
	pattern* retval = 0 ;
	string patt_name = ght[ T::type_id() ];
	string tag_o = strfmt("<%s>", patt_name.c_str() );
	string tag_e = strfmt("</%s>", patt_name.c_str() );
	size_t tag_o_len = tag_o.length();
	size_t tag_e_len = tag_e.length();
	
	if ( str.compare (0, tag_o_len, tag_o) != 0 ) return 0;
	
	struct simple_xml_nested_tag_struct {
		size_t  pos;
		bool    is_tag_o;
		int     lvl; // level
		};
	
	vector<simple_xml_nested_tag_struct>  simple_xml_nested_tags;
	size_t e = 0;
	while(e != std::string::npos) {
		size_t e1 = str.find(tag_o, e);
		size_t e2 = str.find(tag_e, e);
		size_t enext = min(e1,e2);
		size_t enext_len = 0;
		if ( enext == std::string::npos ) break;
		simple_xml_nested_tag_struct xsts;
		if (enext == e1) {
			xsts.pos = e1; 
			xsts.is_tag_o = true; 
			enext_len += tag_o.length();
			}
		else if (enext == e2) {
			xsts.pos = e2; 
			xsts.is_tag_o = false; 
			enext_len += tag_e.length();
			}
		
		xsts.lvl = (simple_xml_nested_tags.size() ? simple_xml_nested_tags.back().lvl : 0) ;
		xsts.lvl += (xsts.is_tag_o ? 1 : -1); 
		simple_xml_nested_tags.push_back(xsts);
		e = enext + enext_len ;
		if ( xsts.lvl == 0 ) break;
		}
	
	if (simple_xml_nested_tags.back().lvl != 0) {
		msgf(VL_FATAL, "Not well-formed."); 
		}
		
	ngram_pattern* np = new ngram_pattern;
	string extracted = str.substr(tag_o_len, e - tag_e_len);
	size_t b = np->unserialise_proper(extracted , 0);
	
	retval = np;
	
	return retval;
	}

iptr<pattern>  pattern::unserialise(const string& str) {
	iptr<pattern> retval ;
	
	for(size_t b=0; b<str.size(); ++b) {
		retval = try_unserialise_pattern<ngram_pattern>(str);
		}

	return retval;
	}*/
