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

#include "bprof.h"
#include <stdio.h>
#include <assert.h>
#include <math.h>

binary_profile::binary_profile(const vector<double>& v)
	: vector<bool>(), __iptr_member() {
	clear();
	for(unsigned int i=0; i<v.size(); ++i) push_back( v[i]==0.0 ? false: true);
	}


string binary_profile::digest() const {
	char buf[ size() + 1 ];
	unsigned char prof = 0x0;
	unsigned char mask  = 0x80;
	char* x = buf;
	for (const_iterator k=begin(); k!=end(); ++k) {
		if( *k ) prof |= (mask & 0xff);
		mask >>= 1;
		if ( mask == 0 ) {
			x += sprintf(x, "%02X", prof);
			prof = 0;
			mask = 0x80;
			}
		}
	if (mask!=0x80) x += sprintf(x, "%02X", prof);
	return string(buf);
	}

string binary_profile::bin_digest() const {
	char buf[ size() + 1 ];
	int i=0;
	for (const_iterator k=begin(); k!=end(); ++k, ++i) {
		buf[i] = *k ? '1' : '0';
		}
	buf[i] = 0;
	return string(buf);
	}

unsigned int binary_profile::npos() const {
	unsigned int cnt = 0;
	for(const_iterator i=begin(); i!=end(); ++i)  if ( *i ) ++cnt ;
	return cnt;
	}

binary_profile::operator vector<double> () const {
	vector<double> retval;
	for(const_iterator i=begin(); i!=end(); ++i) retval.push_back ( *i ? 1.0 : 0.0);
	return retval;
	}


binary_profile::binary_profile(const char* hexdigest): vector<bool>(), __iptr_member() {
	const char* z = hexdigest;
	while(z && *z) {
		unsigned int digit = 0x00 ;
		if ( (*z >= '0') && (*z <= '9') ) {
			digit = *z - '0'; 
			}
		else if ( (*z >= 'A') && (*z <= 'F') ) {
			digit = *z - 'A' + 0x0a; 
			}
		else if ( (*z >= 'a') && (*z <= 'f') ) {
			digit = *z - 'a' + 0x0a; 
			}
		else break;
		unsigned mask = 0x08;
		for(int i=0; i<4; ++i) {
			push_back ( (digit & mask) ? true : false );
			mask >>= 1;
			}
		++z;
		}
	}

bool is_mutex(const binary_profile& b1, const binary_profile& b2) {
	assert(b1.size() == b2.size());
	for(unsigned int i=0; i<b1.size(); ++i) {
		register bool b1i = b1[i] ? 1 : 0;
		register bool b2i = b2[i] ? 1 : 0;
		if ( ! b1i && ! b2i ) continue;
		if ( b1i && b2i ) return false;
		}
	return true;
	}


double binary_profile::normalised_distance(const binary_profile& bp1, const binary_profile& bp2) {
	assert(bp1.size() == bp2.size());
	
	int n= 0;
	for(unsigned int i=0; i<bp1.size(); ++i) {
		if (bp1[i] != bp2[i]) ++n;
		}
	return double(n) / double( bp1.size() );
	}

double binary_profile::eucliean_distance(const binary_profile& bp1, const binary_profile& bp2) {
	assert(bp1.size() == bp2.size());
	
	int n= 0;
	for(unsigned int i=0; i<bp1.size(); ++i) {
		if (bp1[i] != bp2[i]) ++n;
		}
	return sqrt( double(n) );
	}

#include "stats.h"

double binary_profile::fisher_exact_pval(const binary_profile& bp1, const binary_profile& bp2) {
	assert(bp1.size() == bp2.size());
	
	int a=0, b=0, c=0, d=0;
	for(unsigned int i=0; i<bp1.size(); ++i) {
		if (bp1[i]) {
			if(bp2[i]) ++a; else ++b;
			}
		else {
			if(bp2[i]) ++c; else ++d;
			}
		}
	
	return 1.0 - fisher_exact_test(a, b, c, d);
	}


// inline double cc(double x) { if (x < 0.5) return 0.5; return x; }
// inline double sq(double x) { return x * x; }
// double diff(const binary_profile& bp1, const binary_profile& bp2) {
// 	assert(bp1.size() == bp2.size());
// 	
// 	int a=0, b=0, c=0, d=0;
// 	for(int i=0; i<bp1.size(); ++i) {
// 		if (bp1[i]) {
// 			if(bp2[i]) ++a; else ++b;
// 			}
// 		else {
// 			if(bp2[i]) ++c; else ++d;
// 			}
// 		}
// 	int ac=a+c;
// 	int bd=b+d;
// 	int ab=a+b;
// 	int cd=c+d;
// 	int n=a+b+c+d;
// 	double ea= cc(ac) * cc(ab) / sq(n);
// 	double eb= cc(ab) * cc(bd) / sq(n);
// 	double ec= cc(ac) * cc(cd) / sq(n);
// 	double ed= cc(bd) * cc(cd) / sq(n);
// 	double chisq =
// 		( sq(a - ea) / cc(ea) ) +
// 		( sq(b - eb) / cc(eb) ) +
// 		( sq(c - ec) / cc(ec) ) +
// 		( sq(d - ed) / cc(ed) ) ;
// 	
// 	return chisq;
// 	}
