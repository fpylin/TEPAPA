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

#ifndef __tepapa_tester_h
#define __tepapa_tester_h 1

#include "tepapa-gvar.h"
#include "tepapa-evaluator.h"
#include "ngram-pattern.h"
#include <typeinfo>

struct TEPAPA_Tester_profile_set {
	binary_profile   bp;
	vector<double>   vs;
	};

class TEPAPA_Tester: public __iptr_member {
	sample_list*            sl_ptr;
	vector<double>          v_score;
	iptr<TEPAPA_Evaluator>  evaluator;
	vector< iptr<pattern> > patterns_to_test;
	
	protected:
	bool f_use_wildcard;
	bool f_use_deep_cmp;
	
	virtual void run1(unsigned int pstart, unsigned int pend) ;

	public:
	TEPAPA_Tester();
	virtual ~TEPAPA_Tester() {}

	void set_sample_list(sample_list& p_sl_ref);
	void set_use_wildcard(bool pf_use_wildcard) { f_use_wildcard = pf_use_wildcard ; }
	void set_use_deep_cmp(bool pf_set_use_deep_cmp) { f_use_deep_cmp = pf_set_use_deep_cmp; }
	
	template <typename T> void set_test_patterns(const vector<T>& input) {
		for(unsigned int i=0; i<input.size(); ++i) {
			iptr<pattern> ip = input[i];
			patterns_to_test.push_back(ip);
			}
		}
	template <typename T> void set_test_patterns(const vector< iptr<T> >& input) {
		for(unsigned int i=0; i<input.size(); ++i) {
			iptr<pattern> ip = input[i];
			patterns_to_test.push_back(ip);
			}
		}

	virtual bool match_type(pattern* pp)  { return false; }
	
	virtual bool test(TEPAPA_Tester_profile_set& out, const sample_list& sl, pattern* patt_ptr) =0;
	
	virtual int run() ;
	
	virtual const TEPAPA_Results& get_results() const { return evaluator-> get_results() ; }
	};




#include "regex.h"

	
template <typename T> class TEPAPA_Tester_Matchable: public TEPAPA_Tester {
	public:
	TEPAPA_Tester_Matchable(): TEPAPA_Tester() {}
	virtual ~TEPAPA_Tester_Matchable() {}
	
	virtual bool match_type(pattern* pp)  { return (typeid(*pp) == typeid(T)); }
	
	virtual bool test(TEPAPA_Tester_profile_set& out, const sample_list& sl, pattern* patt_ptr) {
		
		const T* spec_patt_ptr = dynamic_cast<const T*>(patt_ptr);
		
		if ( ! spec_patt_ptr ) return false;
		
		out.bp = spec_patt_ptr ->match(sl); 
		
		return true;
		}

	};
	

template <typename T> class TEPAPA_Tester_Evaluable: public TEPAPA_Tester {
	public:
	TEPAPA_Tester_Evaluable(): TEPAPA_Tester() {}
	virtual ~TEPAPA_Tester_Evaluable() {}
	
	virtual bool match_type(pattern* pp)  { return (typeid(*pp) == typeid(T)); }
	
	virtual bool test(TEPAPA_Tester_profile_set& out, const sample_list& sl, pattern* patt_ptr) {
		
		const T* spec_patt_ptr = dynamic_cast<const T*>(patt_ptr);
		
		if ( ! spec_patt_ptr ) return false;
		
		out.bp = spec_patt_ptr ->match(sl); 
		
		out.vs = spec_patt_ptr->eval( sl, out.bp ) ;
		
		return true;
		}
	};



#include "tepapa-program.h"

class TEPAPA_Program_Tester: public TEPAPA_Program {
	bool f_use_wildcard ;
	bool f_use_deep_cmp ;
	bool f_calc_concordance ;
	
	string ds_import_patterns;
	
	vector< iptr<pattern> >  patterns_to_test;
	
	protected: 
	void calc_concordance(const TEPAPA_Results& results) const;
	virtual int run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) ;
	
	void set_test_patterns_from_dataset(const string& ds_name);
	
	public:
	TEPAPA_Program_Tester();

	virtual TEPAPA_Program* spawn() const { return new TEPAPA_Program_Tester; }
	};



#endif //  __tepapa_tester_h 1
