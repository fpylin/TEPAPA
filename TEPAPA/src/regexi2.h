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
#ifndef __regexi2_h
#define __regexi2_h 1

#include <assert.h>

#include <vector>
#include "regex.h"
#include "utils.h"
#include "bprof.h"

using namespace std;


	
struct msa_buffer: public vector<sequence> {
	msa_buffer(): vector<sequence>() {}
	msa_buffer(int n): vector<sequence>(n) {}
	
	sequence get_flattened_aligned_sequence() const;
	vector<binary_profile> gen_aligned_bprofs() const ;
	msa_buffer compress_gaps() const ;
	msa_buffer subset(const binary_profile& bp) const ;
	void print_alignment() const;
	
	void insert_gap(int index);
	};


#if TEPAPA_MULTITHREAD
#include <mutex>
#endif // TEPAPA_MULTITHREAD
	
struct regex_list: public vector<regex> {
	
	regex_list(): vector<regex>() {}
	
	void append(const vector<regex>& rl);
	};

regex do_regex_induction(const msa_buffer& mb);

regex_list do_regex_induction2(const msa_buffer& mb) ;

msa_buffer  do_multiple_align(const vector< sequence >& p_vvh) ;

vector< vector< vector<hash_value> > >  group_ngram_patterns(const vector< vector<hash_value> >& vvh);

regex_list regex_inducer(const sequences& vvh);

#include "ngram-pattern.h"

regex_list regex_inducer(const vector< ngram_pattern >& vvh);


#endif // #ifndef __regexi2_cc
