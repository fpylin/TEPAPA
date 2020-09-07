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

#ifndef __tepapa_results_h
#define __tepapa_results_h 1

#include "hashtable.h"
#include "iptr.h"
#include "bprof.h"
#include "pattern.h"
#include <mutex>

struct TEPAPA_Result {
	double                est;    // estimate
	double                pval;   // pvalue
	int                   dir;    // direction of result >0 = positive, <0 = negative; ==0 = neither
	int                   npos;   // number of positive 
	int                   group;  // group id
	hash_value            method; // evaluation method
	iptr<pattern>         patt;   // pattern
	binary_profile        bprof;  // binary profile
	vector<double>        val;    // values (only used in numeric classes)
	bool                  sel;    // selected (e.g., as an informarker)
  
        double                sens; // sensitivity (binary class only)
        double                spec; // specificity (binary class ounly)
	
        TEPAPA_Result() { est=0; pval=0; dir=0; npos=0; group=0; method=0; sel=false; sens = 0; spec = 0; }
	
	bool is_numeric() const { return (val.size() > 0); }

	int cmp (const TEPAPA_Result& r1, const char field='p') const;
	int cmp (const TEPAPA_Result& r1, const string& cmp_fields) const;
	
	virtual string to_string(const string& fmt="gdmepnx") const;
	
	void reorder_samples(const vector<int>&  o);
	
	template <typename T> T get_signature() const;
	};




typedef pair<hash_value, hash_value>  TEPAPA_method_pattern_pair;



struct pval_est_pair: public pair<double,double> {
	pval_est_pair(double a, double b): pair<double,double>(a,b) {}
	string to_string() const ;
	};


#define TEPAPA_RESULT_FILTER_GT  1
#define TEPAPA_RESULT_FILTER_LT  2
#define TEPAPA_RESULT_FILTER_NE  3
#define TEPAPA_RESULT_FILTER_EQ  4
#define TEPAPA_RESULT_FILTER_GE  5
#define TEPAPA_RESULT_FILTER_LE  6


class TEPAPA_Results: public vector<TEPAPA_Result> {
// 	bool sort_by_pval(const TEPAPA_Result& a, const TEPAPA_Result& b ) ;
	static std::mutex  append_mutex;
// 	std::mutex  append_mutex_1;
	
	public:
	TEPAPA_Results();
	virtual ~TEPAPA_Results() {}
	virtual void save() const {}
	virtual void load() const {}
	
	void dump(const string& fmt="gdmepntbx") const;
// 	void dump(const char* fmt_est = "%-10.8f") const;
	void dump_tcs(const binary_profile& gs) const;
	void sort_by_pval() ;
	void sort_by_est() ;
	void sort(const string& order_by="p") ;

	TEPAPA_Results filter(const char field, const unsigned int cmp, double value) const;
	
	void append(const TEPAPA_Results& rr);
	void push_back_safe(const TEPAPA_Result& r);
	
	int get_unique_binary_profiles() const;
	void reorder_samples(const vector<int>&  o) ;
	
	map<TEPAPA_method_pattern_pair, vector<TEPAPA_Result>::const_iterator>  get_index(void) const;
	
	void reorder_results(const TEPAPA_Results& templ) ;
	
	TEPAPA_Results subset(const binary_profile& bp) const; 
	
	TEPAPA_Results collapse_subpatterns() const ;
	
	void label_group_id(int group_id) ;
	
	template <typename T>  map<T, TEPAPA_Results>  group_by_signature() const {
		map<T, TEPAPA_Results>  out;
		for(const_iterator it=begin(); it!=end(); ++it) {
			out[ it->get_signature<T>() ].push_back( *it );
			}
		return out;
		}
	
	
	template <typename T>  void label_by_signature() {
		map<T, vector<iterator> >  omap;
		for(iterator it=begin(); it!=end(); ++it) {
			omap[ it->get_signature<T>() ].push_back( it );
			}
			
		typename map<T, vector<iterator> >::iterator i;
		int group_id =1;
		for(i=omap.begin(); i!=omap.end(); ++i, ++group_id) {
			for ( vector<iterator>::iterator j=i->second.begin(); j!=i->second.begin(); ++j) {
				(*j)->group = group_id;
				}
			}
		}
	
	map<string, TEPAPA_Results>  group_by_signature(const string& fmt) const ;
	void label_by_signature(const string& fmt) ;
	};


#endif // __tepapa_results_h
