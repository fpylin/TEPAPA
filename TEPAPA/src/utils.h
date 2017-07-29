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
#ifndef __utils_h
#define __utils_h 1

#include <string>
#include <vector>
#include <iostream>

using namespace std;

template <typename T> void print(const vector<T>& array) {
	for(unsigned int i=0; i<array.size() ;++i) {
		if (i) std::cout << " ";
		std::cout << array[i];
		}
	std::cout << "\n";
	}

template <typename T> vector<T> seq(T start, T end, T by=1) {
	vector<T> retval;
	for(T i=start; i<=end; i+=by) retval.push_back(i);
	return retval;
	}

template <typename T>  vector< vector<T> > permute(const vector< vector<string> >&  vv) {
	vector< vector<T> > retval;
	int total = 1;
	int N = vv.size();
	for(int i=0; i<N; ++i) total *= vv[i].size(); 
	for(int z=0; z<total; ++z) {
		vector<T> row(N);
		int n = z;
		for(int i=0; i<N; ++i) {
			int m = N-i-1;
			int d = vv[m].size() ;
			int x = n % d;
			n /= d;
			row[m] = vv[m][x];
			}
		retval.push_back( row );
		}
	return retval;
	}

// template <typename T> T min(T v1, T v2) { return (v1 < v2) ? v1 : v2; }

bool file_exists (const string& path) ;
bool dir_exists (const string& path) ;
string dirname_r(const string& path) ;
string basename_r(const string& path) ;
time_t mtime(const string& path) ;
size_t filesize(const string& path) ;


string load_file(const string& path) ;
bool save_file(const string& path, const string& data) ;



#endif // __utils_h
