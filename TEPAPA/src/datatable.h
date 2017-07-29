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
#ifndef __csvpp_h
#define __csvpp_h 1

#include <vector>
#include <map>
#include <string>

using namespace std;

class data_table {
	map<string, unsigned int>  ch;
	vector<string>             fields;
	vector<char>               field_type; // c = 'categorical' ; 'n' = numeric;
	vector< vector<string> >   field_levels;
	vector< vector<string> >   data;
	
	void field_exists(const map<string, unsigned int>& s, const string& name) const;
	bool check(const map<string, unsigned int>& s, const string& name) const { return s.find(name) != s.end(); }
	vector<string>  parse_line(const char* buf, const char delim = ',');
	void make_rectangular(void);

	void get_field_levels(void);
	void guess_field_types(void);
	
	public:
	
	data_table() { }
// 	data_table(const char* filename, const char delim = ',');
	
	void clear(void) { data.clear(); }
	bool save(const char* filename, const char delim = ',');
	bool load(const char* filename, const char delim = ',');
	void init(void) ;
	void print(void) const;
	
	unsigned int rows() const { 
		return data.size();
		}
	unsigned int cols() const { 
		if ( fields.size() ) return fields.size() ;
		if ( data.size() ) return data.front().size() ;
		return 0 ;
		}
	
	const vector<string>& get_fields() const;
	const vector<string>& get_levels(const string& c) const ;
	char get_field_type(const string& c) const ;
	
	string& operator() (int r, int c) ;
	const string& operator () (int r, int c) const ;
	
	string& operator () (int r, const string& c) ;
	const string& operator () (int r, const string& c) const ;
	
	vector<string> row(int r) const ;
	
	vector<string> col(int c) const ;
	vector<string> col(const string& c) const ;
	
	void copy(const data_table& src, const vector<string>& columns) ;
	void transform_field_eq(const string& field, const string& value) ;
	};

#endif // #ifndef __csvpp_h
