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
#include "regexi2.h"
#include "tepapa-msg.h"
#include "config.h"
#include "parallel.h"

#include <algorithm>

#define DEBUG 0



inline bool nw_eq(const hash_value& c1, const hash_value& c2) {  return c1 == c2 ; }

#include "align.h"
#include "strmetric.h"


sequence  msa_buffer::get_flattened_aligned_sequence() const {
	sequence  retval;
	
	unsigned int z = front().size();
	
	for(unsigned int i=0; i<z; ++i) {
		bool f_next = false;
		unsigned int j=0;
		hash_value  h =0;
		for(j=0; j<size(); ++j) {
			assert ( (begin()+j)->size() == z );
			h = (*(begin()+j))[i] ;
			if ( h != 0) {
				f_next = true;
				retval.push_back( h );
				break;
				}
			}
		if (f_next) continue;
		
// 		if ( j >= size() ) {
// 			fprintf(stderr, "i=%lu\tj=%lu\tsize=%d\th=%lu\t%s\n", i, j, size(), h, ght[h]);
// 			print_alignment();
// 			}
		assert( j < size() ); 
		}
	return retval;
	}


void msa_buffer::insert_gap(int index) {
	for(iterator j=begin(); j<end(); ++j) {
		sequence::iterator z = j->begin() + index;
		j->insert(z, 0);
		}
	}


void msa_buffer::print_alignment() const {
	printf("\n");
	
	for(unsigned int i=0; i<front().size(); ++i) {
		printf ("%d\t", i);
		for(unsigned int j=0; j<size(); ++j) {
			printf ("%-10s  ", ght[ (*(begin()+j))[i] ]);
			}
		printf("\n");
		}
	}
	
void do_align(msa_buffer& vv0, msa_buffer& vv1) {
	sequence v0 = vv0.get_flattened_aligned_sequence();
	sequence v1 = vv1.get_flattened_aligned_sequence();
	
// 	needleman_wunsch<hash_value>  sw1(10, -4, -1);
// 	needleman_wunsch<hash_value>  sw1(5, -4, -1);
	needleman_wunsch<hash_value>  sw1(1, 0, 0);
	
	alignment<hash_value>  ah = sw1( v0, v1 );
	
	sequence w0;
	sequence w1;
	
	int v0i = 0, v1i=0;
	
#if DEBUG
	printf("=== %s ===\n", __PRETTY_FUNCTION__);
	printf("vv0:"); vv0.print_alignment();
	printf("vv1:"); vv1.print_alignment();
#endif // DEBUG
	
	for(unsigned int i=0; i<ah.size(); ++i) {
		while ( v0i < ah[i].first ) {
			w0.push_back( v0[ v0i ] );
			vv1.insert_gap( w1.size() );
			w1.push_back( 0 );
			v0i++;
			}
		
		while ( v1i < ah[i].second ) {
			vv0.insert_gap( w0.size() );
			w0.push_back( 0 );
			w1.push_back( v1[ v1i ] );
			v1i++;
			}
		
		w0.push_back( v0[v0i] ); v0i++;
		w1.push_back( v1[v1i] ); v1i++;
		
#if DEBUG
// 		printf("Alignment: %d\t%d\t%d\n", i, ah[i].first, ah[i].second);
#endif
		}

	while ( v0i < int( v0.size() ) ) {
		w0.push_back( v0[ v0i ] );
		vv1.insert_gap( w1.size() );
		w1.push_back( 0 );
		v0i++;
		}
	
	while ( v1i < int( v1.size() ) ) {
		vv0.insert_gap( w0.size() );
		w0.push_back( 0 );
		w1.push_back( v1[ v1i ] );
		v1i++;
		}
	
#if DEBUG
	for(int i=0; i<w0.size(); ++i) {
		printf (
			"%d\t%-10s   %-10s  \n", i, 
			ght[ w0[i] ], 
			ght[ w1[i] ]
			);
		}
#endif

	for(unsigned int i=0; i<vv1.size(); ++i) {
		vv0.push_back(vv1[i]);
		}
	
	}


	
#include "matrix.h"


mat get_ld_matrix(const vector< sequence >& vvh) {
	int n = vvh.size();
	mat ld(n, n);
	for(int i=0; i<n; ++i) {
		ld(i,i) = 0;
		for(int j=i+1; j<n; ++j) {
			int d = levenshtein(vvh[i], vvh[j]);
// 			int slen = vvh[i].size() +  vvh[j].size();
			ld(i,j) = d ; // / (slen + 1e-10);
			ld(j,i) = d ; // / (slen + 1e-10);
			}
		}
	return ld;
	}

void get_min_ld(const mat& ld, int& r1, int& r2) {
	int min_i = 0, min_j = 0;
	double min_ld = 9e99;
	for(int i=0; i<ld.r(); ++i) {
		for(int j=i+1; j<ld.c(); ++j) {
			double d = ld(i, j);
			if ( d < min_ld ) {
				min_i = i;
				min_j = j;
				min_ld = d;
				}
			}
		}
	r1 = min_i;
	r2 = min_j;
	}


msa_buffer  do_multiple_align(const sequences&  p_vvh) {
	
	sequences              vvh      = p_vvh;
	vector< msa_buffer >   msa_buf;
	
	for(unsigned int i=0; i<vvh.size(); ++i) {
		msa_buffer  vset;
		vset.push_back(vvh[i]);
		msa_buf.push_back(vset);
		}
	
	while(vvh.size()>1) {
#if DEBUG
		printf("\nvvh.size() = %d\n", vvh.size());
#endif // DEBUG
		mat ld = get_ld_matrix(vvh);
#if DEBUG
		ld.dump("levenshtein");
#endif // DEBUG
		int r1, r2;
		get_min_ld(ld, r1, r2);
#if DEBUG
		printf("Min at (%d, %d)\n", r1, r2);
#endif // DEBUG
		do_align(msa_buf[r1], msa_buf[r2]);
#if DEBUG
		printf("*** Result: ***");
		msa_buf[r1].print_alignment();
#endif // DEBUG
		msa_buf.erase( msa_buf.begin() + r2 );
		vvh[r1] = msa_buf[r1].get_flattened_aligned_sequence();
		vvh.erase( vvh.begin() + r2 );
		}

	return msa_buf[0];
	}


vector<binary_profile> msa_buffer::gen_aligned_bprofs() const {
	sequence   aligned = get_flattened_aligned_sequence();
	
	vector<binary_profile>  bprofs ( aligned.size() );
	
	for (unsigned int i=0; i<aligned.size(); ++i) {
		bprofs[i].resize( size() );
		for(unsigned int j=0; j<size(); ++j) {
			bprofs[i][j] = ( ( (*(begin()+j))[i] != 0 ) ? true : false );
			}
#if DEBUG
		printf("%d\t%s\n", i, bprofs[i].digest().c_str() );
#endif
		}
	
	return bprofs;
	}



regex do_regex_induction(const msa_buffer& mb) {
	sequence  aligned = mb.get_flattened_aligned_sequence();
	vector<binary_profile> bprof = mb.gen_aligned_bprofs(); 

	regex               retval;
	binary_profile      bp( mb.size() ) ;
	
	for(unsigned int i=0; i<aligned.size(); ++i) {
		if ( retval.size() == 0 ) retval.push_back( regex_element() ); 
		
		if ( (i==0) || (bprof[i] == bprof[i-1] ) ) {
			if (retval.rbegin()->options.size() == 0) 
				retval.rbegin()->options.push_back_unique( sequence() );
			}
		else if ( is_mutex(bp, bprof[i]) ) {
			retval.rbegin()->options.push_back_unique( sequence() );
			}
		else {
			if ( bp.npos() != mb.size() )
				retval.rbegin()->options.push_back_unique( sequence() );
			retval.push_back( regex_element() );
			retval.rbegin()->options.push_back_unique( sequence() );
			bp.reset();
			}
		retval.rbegin()->options.rbegin()->push_back( aligned[i] );
		bp |= bprof[i];
// 		printf("%d\t%s\n", i, bp.bin_digest().c_str());
		}
	
	if ( bp.npos() != mb.size() ) {
		retval.rbegin()->options.push_back_unique( sequence() );
		}
		
	retval.simplify();
	return retval;
	}





int sim(const vector<hash_value>& sspa, const vector<hash_value>& sspb) {
	vector<hash_value> ssp0 = sspa;
	vector<hash_value> ssp1 = sspb;
	sort( ssp0.begin(), ssp0.end() );
	sort( ssp1.begin(), ssp1.end() );
	unsigned int i=0, j=0;
	int neq = 0;
	while( (i < ssp0.size()) && (j < ssp1.size() ) ) {
		if ( ssp0[i] < ssp1[j]) {
			++i;
			}
		else if ( ssp0[i] > ssp1[j]) {
			++j;
			}
		else { // (ssp0[i] == ssp0[j]) 
			++neq;
			++i;
			++j;
			}
		}
	return neq;
	}

	
int bilaterally_identical(const vector<hash_value>& sspa, const vector<hash_value>& sspb) {
	if ( sspa.size() < 2 ) return 0;
	if ( sspb.size() < 2 ) return 0;
	if ( sspa[0] != sspb[0] ) return 0;
	if ( sspa[ sspa.size() - 1 ] != sspb[ sspb.size() - 1 ] ) return 0;
	return 1;
	}
	
void print_alignment(const vector<hash_value>& vh) {
	for(unsigned int k=0; k<vh.size(); ++k) {
		if (k) printf(" ");
		printf("%s", ght[ vh[k] ] );
		}
	}


msa_buffer msa_buffer::compress_gaps() const {
	msa_buffer retval( size() );
	
	for(unsigned int i=0; i<front().size(); ++i) {
		unsigned int j=0;
		for(j=0; j<size(); ++j) {
			if ( (*(begin()+j))[i] ) break;
			}
		if ( j >= size() ) continue;
		
		for(unsigned int k=0; k<size(); ++k) {
			retval[k].push_back( (*(begin()+k))[i] );
			}
		}
	return retval;
	}

	
msa_buffer msa_buffer::subset(const binary_profile& bp) const {
	msa_buffer  retval;
	assert(size() == bp.size());
	
	for(unsigned int i=0; i<size(); ++i) {
		if (bp[i]) retval.push_back( *(begin()+i) );
		}
	return retval;
	}


regex_list do_regex_induction2(const msa_buffer& mb) {
	
	vector<hash_value>       aligned = mb.get_flattened_aligned_sequence();
	vector<binary_profile>   aligned_bprof = mb.gen_aligned_bprofs(); // ( aligned.size() ) ;
	regex_list               retval;
	
	binary_profile bp_front( mb.size() );
	
	for(unsigned int i=0; i<aligned_bprof.size(); ++i) {
		bool f_mutex = is_mutex(bp_front, aligned_bprof[i]);
		bool f_eq = ( (i>0) && (aligned_bprof[i] == aligned_bprof[i-1] )) ? true : false;
// 		printf("F\t%d\t%s\t%s\t%d\t%d\n", i, bp_front.digest().c_str(), aligned_bprof[i].digest().c_str(), f_mutex, f_eq);
		if ( ! f_mutex && ! f_eq ) break;
		bp_front |= aligned_bprof[i];
		}

	msa_buffer  mb1 = mb.subset(bp_front);
	msa_buffer  mb1c = mb1.compress_gaps();
	
// 	mb1c.print_alignment();
	regex  r = do_regex_induction(mb1c);
// 	printf ("F> %s\n", r.to_str().c_str() );
	
	if ( mb1c.size())  { 
		binary_profile         bp_back( mb1c.size() );
		sequence               mb1c_aligned       = mb1c.get_flattened_aligned_sequence();
		vector<binary_profile> mb1c_aligned_bprof = mb1c.gen_aligned_bprofs();
		
		int mb1c_last = mb1c_aligned_bprof.size()-1;
		for(int i=mb1c_last ; i>=0; --i) {
			bool f_mutex = is_mutex(bp_back, mb1c_aligned_bprof[i]);
			bool f_eq = ( (i<mb1c_last) && (mb1c_aligned_bprof[i] == mb1c_aligned_bprof[i+1] )) ? true : false;
// 			printf("B\t%d\t%s\t%s\t%d\t%d\n", i, bp_back.digest().c_str(), vvh1c_aligned_bprof[i].digest().c_str(), f_mutex, f_eq);
			if ( ! f_mutex && ! f_eq ) break;
			bp_back |= mb1c_aligned_bprof[i];
			}
		msa_buffer  mb1cb = mb1c.subset(bp_back);
		msa_buffer  mb1cbc = mb1cb.compress_gaps();
		
// 		vvh1cbc.print_alignment();
		regex  r = do_regex_induction(mb1cbc);
// 		printf ("B> %s\n", r.to_str(ht).c_str() );
		retval.push_back(r);
		
		msa_buffer  mb1c2 = mb1c.subset(!bp_back);
		
		if ( mb1c2.size() > 0 ) {
			msa_buffer  mb1c2c = mb1c2.compress_gaps();
			
			vector<regex> retval2 = do_regex_induction2(mb1c2c);
			
			retval.append( retval2 );
			}
		}
	
	msa_buffer  mb2 = mb.subset( !bp_front );
	if ( mb2.size() > 0 ) {
		msa_buffer  mb2c = mb2.compress_gaps();
		
		vector<regex> retval2 = do_regex_induction2(mb2c);
		
		retval.append( retval2 );
		}
	
	return retval;
	}



vector<sequences>  group_ngram_patterns(const sequences& seqs) {
	
	map< pair<hash_value, hash_value>, sequences >  seq_groups_map;
	
	for(unsigned int i=0; i<seqs.size(); ++i) {
// 		if (i % 20000 == 0) fprintf(stderr, "Grouping %u sequences.\n", i);
		if (seqs[i].size() < 3) continue;
		hash_value head = seqs[i].front();
		hash_value tail = seqs[i].back();
		
		pair<hash_value,hash_value> phv = pair<hash_value,hash_value>(head, tail);
		
		seq_groups_map[phv].push_back( seqs[i] );
		}

	vector<sequences>  seq_groups; // output
	
	static int group_id = 0;
	
	msgf(VL_DEBUG, "Regex groups:\n");
	
	for(map< pair<hash_value, hash_value>, sequences>::iterator it=seq_groups_map.begin(); it!=seq_groups_map.end(); ++it) {
		
		const sequences& org = it->second;
		++group_id;
		msgf(VL_DEBUG, "Groups %d size %d\n", group_id, org.size() );
		
		if ( org.size() > 4 ) {
			if ( org.size() < 100 ) 
				msgf(VL_DEBUG, "%s\n", org.to_str().c_str() );
			else
				msgf(VL_DEBUG, "(%d patterns)\n", org.size() );
// 			fprintf(stderr, "!0");
			
			map<hash_value,int>  token_count_map = org.count_tokens();
			vector<hash_value>   sorted_token_list ;
			for(auto it = token_count_map.begin(); it != token_count_map.end(); ++it ) sorted_token_list .push_back( it->first );
			sort(sorted_token_list.begin(), sorted_token_list.end(), 
				[&](const hash_value& a,const hash_value& b)->bool {
					return token_count_map[b] < token_count_map[a];
				});
			
			msgf(VL_DEBUG, "\t%s\t%s\n", "Count", "Symbol sequence");
			
			hash_value hv_sep = 0; // find the best place to cut the group FIXME: ideally should use information
			
			for(auto z=sorted_token_list.begin(); z!=sorted_token_list.end(); ++z) {
				msgf(VL_DEBUG, "\t%d\t%s\n", token_count_map[*z], ght[*z]);
				if (token_count_map[*z] == 1 ) break; // no point grouping a singleton
				if ( token_count_map[*z] != (int)(org.size()) ) { 
					hv_sep = *z;
					break;
					}
				// here: no point grouping by a token that is presnet in all sequences
				}
		
			if ( hv_sep != 0 ) {
				sequences group1, group2;
				
				org.split_by(hv_sep, group1, group2);
// 				fprintf(stderr, "!1");
// 				msgf(VL_DEBUG, "Split by [%s]:\n", ght[hv_sep] );
// 				fprintf(stderr, "!2");
// 				msgf(VL_DEBUG, "%s", group1.to_str("  Group 1:").c_str() ) ;
// 				fprintf(stderr, "!3");
// 				msgf(VL_DEBUG, "%s", group2.to_str("  Group 2:").c_str() ) ;
				
				vector<sequences>  group1_result = group_ngram_patterns(group1);
				vector<sequences>  group2_result = group_ngram_patterns(group2);
				
				seq_groups.insert( seq_groups.end(), group1_result.begin(), group1_result.end());
				
				seq_groups.insert( seq_groups.end(), group2_result.begin(), group2_result.end());
				}
			else {
				seq_groups.push_back( org );
				}
			}
		else {
			seq_groups.push_back( org );
			}
		}

	return seq_groups;
	}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void regex_inducer1(regex_list& retval, const vector<sequences>& substr_groups, unsigned int group_start, unsigned int group_end) {
	
	msgf(VL_INFO, "Inducing regular expressions - processing groups %d to %d \n", group_start, group_end);
	
	for (unsigned int g=group_start; g<group_end; ++g) {
		if ( g>=substr_groups.size() ) break;
		if ( substr_groups[g].size() <= 1 ) continue;
		
		msa_buffer  result = do_multiple_align( substr_groups[g] );
		
// 		regex  r = do_regex_induction(result);
// 		printf ("%s\n", r.to_str().c_str() );
#if DEBUG
		printf("\n\n*** Final: ***");
		print_alignment(result);
#endif
		regex_list regices = do_regex_induction2(result);
		
		retval.append( regices );
		}
	}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

regex_list regex_inducer(const sequences& seqs) {
	
	regex_list  retval;
	
	// grouping
	vector< sequences >  substr_groups = group_ngram_patterns(seqs);
	
	msgf(VL_INFO, "%u groups\n", substr_groups.size());
	
	const int unit_size = 1; // 10
	
#if TEPAPA_MULTITHREAD
	thread_manager tm;
#endif // TEPAPA_MULTITHREAD
	retval.reserve( substr_groups.size() );
	
	for (unsigned int g=0; g<substr_groups.size(); g += unit_size) {
#if TEPAPA_MULTITHREAD
		tm.queue();
		tm.push_back( 
			std::async( std::launch::async, &regex_inducer1, std::ref(retval), std::ref(substr_groups), g, g+unit_size) 
			);		
#else	
		regex_inducer1(retval, substr_groups, g, g+unit_size);
#endif // TEPAPA_MULTITHREAD
		}
#if TEPAPA_MULTITHREAD
	tm.join_all();
#endif // TEPAPA_MULTITHREAD
	
	return retval;
	}


regex_list regex_inducer(const vector< ngram_pattern >& vvh) {
	sequences  seqs;
	for(unsigned int i=0; i<vvh.size(); ++i) {
		seqs.push_back( vvh[i] );
		}
	
	return regex_inducer(seqs);
	}

void regex_list::append(const vector<regex>& rl) { 
#if TEPAPA_MULTITHREAD
	static std::mutex regex_list_mutex;
	regex_list_mutex.lock();
#endif // TEPAPA_MULTITHREAD
	for (unsigned int j=0; j<rl.size(); ++j) push_back(rl[j]); 
#if TEPAPA_MULTITHREAD
	regex_list_mutex.unlock();
#endif // TEPAPA_MULTITHREAD
	}
