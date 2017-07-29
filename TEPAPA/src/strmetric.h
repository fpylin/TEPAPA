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
#ifndef __strmetric_h
#define __strmetric_h 1

#include "matrix.h"
#include "hashtable.h"
#include <vector>

template <typename T>  int min(T a, T b, T c) {
	T x = a;
	if (b < x) x = b;
	if (c < x) x = c;
	return x;
	}


template <typename T>  int levenshtein(const vector<T>& s1, const vector<T>& s2) {
	int len1 = s1.size();
	int len2 = s2.size();
	if (len1 == 0) return len2 ;
	if (len2 == 0) return len1 ;

	imat m(len1+1, len2+1);
	
	for (int i=0; i<=len1; ++i) {
		for (int j=0; j<=len2; ++j) {
			m(i,j) = 0;
			m(0,j) = j;
			}
		m(i,0) = i;
		}

	for (int i= 1; i<=len1; ++i) {
		for (int j = 1; j <= len2; ++j) {
			int cost = (s1[i-1] == s2[j-1]) ? 0 : 1;
			m(i,j) = min( 
				m(i-1,j) + 1, 
				m(i,j-1) + 1,
				m(i-1,j-1) + cost 
				);
			}
		}

	return m(len1,len2);
	}


template <typename T>  int damerau_levenshtein(const vector<T>& s1, const vector<T>& s2) {
	int len1 = s1.size();
	int len2 = s2.size();
	if (len1 == 0) return len2 ;
	if (len2 == 0) return len1 ;

	imat m(len1+1, len2+1);
	
	for (int i=0; i<=len1; ++i) {
		for (int j=0; j<=len2; ++j) {
			m(i,j) = 0;
			m(0,j) = j;
			}
		m(i,0) = i;
		}

	for (int i= 1; i<=len1; ++i) {
		for (int j = 1; j <= len2; ++j) {
			int cost = (s1[i-1] == s2[j-1]) ? 0 : 1;
			m(i,j) = min( 
				m(i-1,j) + 1,
				m(i,j-1) + 1,
				m(i-1,j-1) + cost 
				);
			if ( (i > 1) && (j > 1) && s1[i] == s2[j-1] && s1[i-1] == s2[j] ) {
                m(i, j) = min( 
					m(i, j),
					m(i-2, j-2) + cost
					) ;
				}
			}
		}

	return m(len1,len2);
	}

#endif // __strmetric_h
