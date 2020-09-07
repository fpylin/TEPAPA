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

#include "tepapa-results.h"
#include "strfriends.h"
#include "tepapa-utils.h"
#include "stats.h"
#include "tepapa-msg.h"
#include <algorithm>

string pval_est_pair::to_string() const { 
	return strfmt("<%g-%f>", first, second); 
	}


TEPAPA_Results::TEPAPA_Results(): 
	vector<TEPAPA_Result> () {
// 	reserve(4194304);
	reserve(65536);
	}

template <class T> int cmp3(const T& v1, const T& v2) {
	if (v1 < v2) return -1;
	if (v1 > v2) return  1;
	return 0;
	}

double sgn(int dir) {
	if (dir < 0) return -1.0 ;
	if (dir > 0) return +1.0 ;
	return 0.0;
	}
	
int TEPAPA_Result::cmp (const TEPAPA_Result& r1, const char field) const {
	int d;
	switch(field) {
		case 'g': // group 
			if ( (d = cmp3(group, r1.group)) ) return d;
			break;
		case 'd': // direction of result >0 = positive, <0 = negative; ==0 = neither
			if ( (d = cmp3(r1.dir, dir)) ) return d;
			break;
		case 'm': // evaluation method
			if ( (d = cmp3(method, r1.method)) ) return d;
			break;
		case 'e': // estimate
			if ( (d = cmp3( sgn(r1.dir) * r1.est, sgn(dir) * est )) ) return d;
			break;
		case 'p': // pvalue
			if ( (d = cmp3( pval, r1.pval )) ) return d;
			break;
		case 'n': // number of positive 
			if ( (d = cmp3( r1.npos, npos )) ) return d;
			break;
		case 't': // pattern type 
			if ( (d = cmp3( patt->get_type_id(), r1.patt->get_type_id() )) ) return d;
			break;
		case 'b': // binary profile
			if ( (d = cmp3( bprof, r1.bprof )) ) return d;
			break;
// 		case 'v': // values (only used in numeric classes)
		case 's': // selected (e.g., as an informarker)
			if ( (d = cmp3( sel, r1.sel )) ) return d;
			break;
		case 'x': // pattern
			if ( (d = cmp3( patt->to_string(), r1.patt->to_string() )) ) return d;
			break;
		default:
			msgf(VL_FATAL, "%s, No such field in `%c'", __PRETTY_FUNCTION__, field);
			break;
		}
	return 0;
	}

int TEPAPA_Result::cmp(const TEPAPA_Result& r1, const string& cmp_fields) const {
	for(unsigned int i=0; i<cmp_fields.length(); ++i) {
		int d = cmp(r1, cmp_fields[i]);
		if (d) return d;
		}
	return 0;
	}

	
string TEPAPA_Result::to_string(const string& fmt) const {
	string out ;
	
	for(unsigned int i=0; fmt[i]; ++i) {
		if (i) out += "\t";
		switch(fmt[i]) {
			case 'g': // group 
				out += strfmt("G%04d", group) ;
				break;
			case 'd': // direction of result >0 = positive, <0 = negative; ==0 = neither
				out += ( (dir > 0) ? "POS"  : (dir < 0) ? "NEG" : "---" );
				break;
			case 'm': // evaluation method
				out += ght[ method ] ;
				break;
			case 'e': // estimate
				out += strfmt("%-10.7f", est) ;
				break;
			case 'p': // pvalue
				out += strfmt("%-10.6g", pval);
				break;
			case 'n': // number of positive 
				out += strfmt("%d", npos);
				break;
			case 't': // pattern type 
				out += ght[ patt->get_type_id() ] ;
				break;
			case 'b': // binary profile
				out += bprof.digest();
				break;
// 			case 'v': // values (only used in numeric classes)
			case 's': // selected (e.g., as an informarker)
				out += (sel ? "*" : "");
				break;
			case 'X': // pattern
				out += patt->to_string("").c_str() ; 
				break;
			case 'x': // pattern
				out += patt->to_string().c_str() ; 
				break;
			case 'z': // pcre
				out += patt->gen_pcre().c_str() ; 
				break;
			default:
				out += fmt[i];
				break;
			}
		};
	
// 	out += string("\n") ;
	return out;
	}

int TEPAPA_Results::get_unique_binary_profiles() const {
	set<binary_profile>  bprofs;
	
	for(const_iterator i=begin(); i!=end(); ++i)  bprofs.insert( i->bprof );
	
	return bprofs.size();
	}

std::mutex  TEPAPA_Results::append_mutex;

void TEPAPA_Results::append(const TEPAPA_Results& rr) {
// #if TEPAPA_MULTITHREAD
// 	append_mutex.lock();
// #endif // TEPAPA_MULTITHREAD
	for(unsigned int i=0;i<rr.size();++i) push_back_safe(rr[i]);
// #if TEPAPA_MULTITHREAD
// 	append_mutex.unlock();
// #endif // TEPAPA_MULTITHREAD
	}

void TEPAPA_Results::push_back_safe(const TEPAPA_Result& r) {
#if TEPAPA_MULTITHREAD
	append_mutex.lock();
#endif // TEPAPA_MULTITHREAD
	
	push_back(r);
	
#if TEPAPA_MULTITHREAD
	append_mutex.unlock();
#endif // TEPAPA_MULTITHREAD
	}


void TEPAPA_Results::sort_by_pval() { 
	std::sort( begin(), end(), ::sort_by_pval );  
	}

void TEPAPA_Results::sort_by_est() { 
	std::sort( begin(), end(), ::sort_by_est );  
	}


void TEPAPA_Results::sort(const string& order_by) {
	std::sort( begin(), end(), 
		[&](const TEPAPA_Result& a, const TEPAPA_Result& b) -> bool { 
			return a.cmp(b, order_by) < 0; 
			}
		);  
	}



TEPAPA_Results TEPAPA_Results::filter(const char field, const unsigned int cmp, double value) const {


  TEPAPA_Results retval;
  
  for (const_iterator i=begin(); i!=end(); ++i) {
    double z = 0.0;
    switch(field) {
    case 'D':  z = fabs(i->dir); break;
    case 'd':  z = i->dir; break;
    case 'E':  z = fabs(i->est); break;
    case 'e':  z = i->est; break;
    case 'p':  z = i->pval; break;
    case 'n':  z = i->npos; break;
    default: msgf(VL_FATAL, "%s, No such field in `%c'", __PRETTY_FUNCTION__, field);
      break;
    };
    
    bool tf = false;
    switch(cmp){
    case TEPAPA_RESULT_FILTER_GT: tf = z >  value; break;
    case TEPAPA_RESULT_FILTER_LT: tf = z <  value; break;
    case TEPAPA_RESULT_FILTER_NE: tf = z != value; break;
    case TEPAPA_RESULT_FILTER_EQ: tf = z == value; break;
    case TEPAPA_RESULT_FILTER_GE: tf = z >= value; break;
    case TEPAPA_RESULT_FILTER_LE: tf = z <= value; break;
    default: msgf(VL_FATAL, "%s, Invalid comparator `%d'", __PRETTY_FUNCTION__, cmp);
      break;
    };
    
    if(tf) retval.push_back(*i);
  }
  return retval;
  
}





void TEPAPA_Results::dump(const string& fmt) const {
	for(const_iterator it=begin(); it!=end(); ++it) {
		string s = it->to_string(fmt);
		printf( "%s\n", s.c_str() );
		}
	}

void TEPAPA_Results::dump_tcs(const binary_profile& gs) const {
	for(const_iterator it=begin(); it!=end(); ++it) {
		two_class_stats tcs ;
		two_class_stats::compute( tcs, gs, it->bprof);
		string s = tcs.format("efghabcdjJl");
		s += string("  ") + it->patt->to_string() + string("\n");
		printf( "%s", s.c_str() );
		}
	}

void TEPAPA_Result::reorder_samples(const vector<int>&  o) {
	size_t  sz = o.size();
	assert( sz == bprof.size() );
	assert( ! val.size() || sz == val.size() );
	
	binary_profile  old_bprof( bprof );
	vector<double>  old_val(val );
	
	for(unsigned int i=0; i<sz; ++i) {
		bprof[ o[i] ] = old_bprof[i];
		val[ o[i] ] = old_val[i];
		}
	}

void TEPAPA_Results::reorder_samples(const vector<int>&  o) {
	for(iterator it = begin(); it != end(); ++it) it->reorder_samples(o);
	}

map<TEPAPA_method_pattern_pair, vector<TEPAPA_Result>::const_iterator>  TEPAPA_Results::get_index(void) const {
	map<TEPAPA_method_pattern_pair, vector<TEPAPA_Result>::const_iterator> retval;
	
	for(const_iterator it=begin(); it != end(); ++it) {
		TEPAPA_method_pattern_pair  z(it->method, it->patt->get_uniq_id() );
		retval[ z ] = it;
		}
	return retval;
	}

void TEPAPA_Results::reorder_results(const TEPAPA_Results&  templ) {
	map<TEPAPA_method_pattern_pair, const_iterator>  order_map = get_index(); 
		
	TEPAPA_Results rr_new;
	for(unsigned int i=0; i<templ.size(); ++i) {
		TEPAPA_method_pattern_pair  z(templ[i].method, templ[i].patt->get_uniq_id() );
		if ( order_map.find(z) == order_map.end() ) {
			msgf(VL_FATAL, "Not compatible!");
			}
		rr_new.push_back( *( order_map[ z ] ) );
		}
	*this = rr_new;
	}

TEPAPA_Results TEPAPA_Results::subset(const binary_profile& bp) const {
	TEPAPA_Results retval;
	assert(size() == bp.size());
	for(unsigned int i=0; i<size(); ++i) {
		const_iterator it = (begin() + i) ;
		if (bp[i]) retval.push_back( *it );
		}
	return retval;
	}

void TEPAPA_Results::label_group_id(int group_id) {
	for(iterator it=begin(); it!=end(); ++it)  {
		it->group = group_id;
		}
	}

#include "matchable-pattern.h"

TEPAPA_Results TEPAPA_Results::collapse_subpatterns() const {
	
	TEPAPA_Results v(*this);
	
	std::sort(v.begin(), v.end(), sort_by_ngram_length_desc);
	
	binary_profile  valid( v.size() ) ; 
	
	for(unsigned int j=0; j<v.size(); ++j) valid[j] = true;
	
	for(unsigned int j=0; j<v.size(); ++j) {
		if ( ! valid[j] ) continue;
		pattern* vptr =  v[j].patt;
		matchable_pattern* np_vptr = dynamic_cast<matchable_pattern*>(vptr);
		if ( ! np_vptr ) continue;
		
		for(unsigned int z=j+1; z<v.size(); ++z) {
			pattern* zptr =  v[z].patt;
			if ( ! valid[z] ) continue;
			
			matchable_pattern* np_zptr = dynamic_cast<matchable_pattern*>(zptr);
			if ( ! np_zptr ) continue;
			
			if ( np_vptr->has_subpat(*np_zptr) ) {
				valid[z] = false;
				continue;
				}
			}
		}
		
	return v.subset(valid) ;
	}

	
template <> binary_profile TEPAPA_Result::get_signature() const { return bprof; }
template <> int            TEPAPA_Result::get_signature() const { return group; }
template <> pval_est_pair  TEPAPA_Result::get_signature() const { return pval_est_pair( pval, est ); }

map<string, TEPAPA_Results>  TEPAPA_Results::group_by_signature(const string& fmt) const {
	map<string, TEPAPA_Results>  out;
	for(const_iterator it=begin(); it!=end(); ++it) {
		out[ it->to_string(fmt) ].push_back( *it );
		}
	return out;
	}

void TEPAPA_Results::label_by_signature(const string& fmt) {
	map<string, vector<iterator> >  omap;
	for(iterator it=begin(); it!=end(); ++it) {
		omap[ it->to_string(fmt) ].push_back( it );
		}
		
	typename map<string, vector<iterator> >::iterator i;
	int group_id =1;
	for(i=omap.begin(); i!=omap.end(); ++i, ++group_id) {
// 		printf("%s\n", i->first.c_str());
		for ( vector<iterator>::iterator j=i->second.begin(); j!=i->second.end(); ++j) {
// 			printf("\t%s\n", (*j)->to_string().c_str());
			(*j)->group = group_id;
			}
		}
	}
