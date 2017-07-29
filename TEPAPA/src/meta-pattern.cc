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
#include "meta-pattern.h"
#include "ngram-pattern.h"

hash_value meta_pattern::meta_pattern_hv_type_id;

string meta_pattern::to_string(const string& delim) const {
	string retval;
	retval += string("(") + string( ght[hv_op] );
	for(const_iterator it = begin(); it != end(); ++it) {
		const pattern* p = *it;
		retval += " [" + string( ght[ p->get_type_id() ] ) + string(":");
		retval += p->to_string(delim) + "]";
		}
	retval += (")");
	return retval;
	}

bool meta_pattern::match(const token_string&  ts) const {
	if ( hv_op == ght("OR") ) {
		for(const_iterator it = begin(); it != end(); ++it) {
			const pattern* p = *it;
			const matchable_pattern* mp = dynamic_cast<const matchable_pattern*>(p) ;
			if (! mp) continue;
			if (mp->match(ts)) return true;
			}
		return false;
		}
	
	if ( hv_op == ght("AND") || hv_op == ght("FOLLOW") ) {
		for(const_iterator it = begin(); it != end(); ++it) {
			const pattern* p = *it;
			const matchable_pattern* mp = dynamic_cast<const matchable_pattern*>(p) ;
			if (! mp ) return false ;
			if (! mp->match(ts) ) return false;
			}
		if ( hv_op == ght("AND") ) return true;

		// Deal with "FOLLOWS" pattern:
		vector< vector<int> > v_zmpi;
		for(const_iterator it = begin(); it != end(); ++it) {
			const pattern* pi = *it;
			const ngram_pattern* mpi = dynamic_cast<const ngram_pattern*>(pi) ;
			if (! mpi ) continue;
			vector<int>  zmpi =  mpi->find_all(ts);
			v_zmpi.push_back(zmpi);
			}
		
		for(unsigned i=0; i<v_zmpi.size(); ++i) {
			for(unsigned j=i+1; j<v_zmpi.size(); ++j) {
				for(unsigned k=0; k<v_zmpi[i].size(); ++k) {
					for(unsigned l=0; l<v_zmpi[j].size(); ++l) {
						if (v_zmpi[i][k] < v_zmpi[j][l]) return true;
						}
					}
				}
			}
		
		return false;
		}
	
	return false;
	}


double meta_pattern::atfidf(const sample_list& sl) const { 
	double sum = 0;
	for(const_iterator it = begin(); it != end(); ++it) {
		const pattern* pp = (*it);
		
		const ngram_pattern* ngpp = dynamic_cast<const ngram_pattern*>(pp);
		if (ngpp) { sum += ngpp->atfidf(sl); continue; }
		
		const meta_pattern* mpp = dynamic_cast<const meta_pattern*>(pp);
		if (mpp) { sum += mpp->atfidf(sl); continue; }
		}
	
	if ( hv_op == ght("OR") ) 
		return sum / double( size() );
	else 
		return sum;
	}
