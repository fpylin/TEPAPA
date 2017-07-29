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
#ifndef __stats_h
#define __stats_h 1

#include <stdio.h>
#include <math.h>
#include <assert.h>

using namespace std;

#include <string>
#include <vector>
#include <algorithm>


#define DIV_CORRECTION   1e-10

extern vector<double> hypergeometric_log_i_cache;
extern void hypergeometric_log_i_cache_init(void);

vector<double> LOG(const vector<double>& v) ;
double mean(const vector<double>& v) ;
double sd(const vector<double>& v) ;
double median(const vector<double>& v) ;

double pnorm (double x) ;
long double pnorml (long double x) ;
double qnorm (double p) ;

double fisher_exact_test(int a, int b, int c, int d);
double odds_ratio(int a, int b, int c, int d);

// double binom_ci


struct two_class_stats {
	int tp, tn, fp, fn;
	
	void reset (void) {tp = tn = fp = fn = 0; }
	
	/** Constructors */
	two_class_stats() {reset();}
	two_class_stats(int ptp, int ptn, int pfp, int pfn)
		{tp = ptp; tn = ptn; fp = pfp; fn = pfn; }
		
	
	void incr(bool actual, bool test) {
		if(actual) {
			if(test) tp++; else fn++;
			}
		else  {
			if(test) fp++; else tn++;
			}
		}
	
	int dp() const {return tp+fn;} // diagnosis positive
	int dn() const {return tn+fp;} // diagnosis negative
	int sp() const {return tp+fp;} // test positive
	int sn() const {return tn+fn;} // test negative
	int sc() const {return tn+fn;} // test makes correct diagnosis
	int sw() const {return tn+fp;} // test makes incorrect diagnosis
	
	int tot() const {return tp+fp+tn+fn;} // total
	
	double spr() const {return double(tp+fp)/tot();} // "lift"
	
	double acc() const {return double(tp+tn)/double(tp+fp+tn+fn);} // accuracy
	
	// prevlance
	double prev()  const {return double(tp+fn)/double(tp+fp+tn+fn);}  
	
	// sensitivity and specificity 
	double sens () const {return double(tp)/(double(dp()));}
	double spec () const {return double(tn)/(double(dn()));}
	double tpr ()  const {return sens();}
	double fpr ()  const {return 1.00 - spec();}
	
	// predictive values
	double ppv ()  const {
		double v_adj = ( tp==0 /*|| fp==0*/ ) ? 0.5 : 0;
		return (double(tp+v_adj) ) / ( double(tp+fp+v_adj*2) );
		}
		
	double npv () const {
		double v_adj = ( tn==0 /*|| fn==0*/ ) ? 0.5 : 0;
		return (double(tn+v_adj) ) / ( double(tn+fn+v_adj*2) );
		}
		
	double ppv (double prev)  const { // by Bayes' theorem
		return sens()*prev / (sens()*prev + (1-spec())*(1-prev));
		}
	double npv (double prev)  const { // by Bayes' theorem
		return spec()*(1-prev) / ( spec()*(1-prev) + (1-sens()*prev) );
		}
	
	// harmonic measure
	static double harmonic(double v1, double v2) {
		if (v1 == 0.00) v1 = 1e-100;
		if (v2 == 0.00) v2 = 1e-100;
		return 2.00/(1.00/v1+1.00/v2);
		}
		
	double f_measure() const {return harmonic( sens(), ppv() );}
	double h_measure() const {return harmonic( sens(), spec() );}
	
	double adj(int x) const { return x ? x : x+0.5; }
	
	// Odds ratio
	double o_r() const {
// 		double v_adj = ( (fn == 0) || (tp+fn==0) || (tn ==0) || (fp+tn==0) ) ? 0.5 : 0;
		double v_adj = ( (fn == 0) || (tp ==0) || (tn ==0) || (fp ==0) ) ? 0.5 : 0;
		return ( (tp + v_adj) * (tn + v_adj) ) /  ( (fp + v_adj) * (fn + v_adj) ) ;
		}
		
	double log_or() const {
// 		double v_adj = ( (fn == 0) || (tp+fn==0) || (tn ==0) || (fp+tn==0) ) ? 0.5 : 0;
		double v_adj = ( (fn == 0) || (tp ==0) || (tn ==0) || (fp ==0) ) ? 0.5 : 0;
		return ( log(tp + v_adj) + log(tn + v_adj) - log(fp + v_adj) - log(fn + v_adj) ) ;
		}
		
	double se_log_or() const {
		double v_adj = ( (fn == 0) || (tp+fn==0) || (tn ==0) || (fp+tn==0) ) ? 0.5 : 0;
		return sqrt( 
			  1.0/(tp + v_adj) + 1.0/(fp + v_adj) 
			+ 1.0/(fn + v_adj) + 1.0/(tn + v_adj) 
			);
		}
	double ci95_l_or(void) const { return exp(log(o_r()) - 1.96 * se_log_or());}
	double ci95_h_or(void) const { return exp(log(o_r()) + 1.96 * se_log_or());}
		
	double rr() const  { return ppv() / (1-npv()); }
	double lrp()  const  {
		double v_adj = ( (tp == 0) || (tp+fn==0) || (fp==0) || (fp+tn==0) ) ? 0.5 : 0;
		return ((tp+v_adj)/(tp+fn+2*v_adj)) / ((fp+v_adj)/(fp+tn+2*v_adj));
		}
	double lrn()  const  {
		double v_adj = ( (fn == 0) || (tp+fn==0) || (tn ==0) || (fp+tn==0) ) ? 0.5 : 0;
		return ((fn+v_adj)/(tp+fn+2*v_adj)) / ((tn+v_adj)/(fp+tn+2*v_adj));
// 		return (1-tpr())/spec();
		}
	
	double se_log_lrp() const {
		double v_adj = ( (tp == 0) || (tp+fn==0) || (fp==0) || (fp+tn==0) ) ? 0.5 : 0;
		return sqrt( 1.0/(tp+v_adj) - 1.0/(tp+fn+v_adj*2) + 1.0/(fp+v_adj) - 1.0/(fp+tn+v_adj*2) );
		}
	
	double ci95_l_lrp(void) const { return exp(log(lrp()) - 1.96 * se_log_lrp());}
	double ci95_h_lrp(void) const { return exp(log(lrp()) + 1.96 * se_log_lrp());}
	
	
	/* Returns empirical AUC / arithematic mean of sensitivity and specificity */
	double eauc()  const {return 0.5 * (sens() + spec());}
	double amss()  const {return 0.5 * (sens() + spec());}
	
	/* Return theoretically the worst AUC as estimated by binormal method */
	double mauc()  const {return min(sens(), spec());}
	

	/* Return statistic as specified by code */
	double operator()(char code) const;
	
	two_class_stats& operator += (const two_class_stats& tm) {
		tp += tm.tp; fp += tm.fp; 
		fn += tm.fn; tn += tm.tn; 
		return *this;
		}
	
	/* output function */
	void print(char fmt, FILE* file=stdout) const;
	void print(const string& output_format, FILE* file=stdout) const;
	string format(char fmt) const;
	string format(const string& output_format) const ;
	
	static void print_help(FILE* file=stdout);
	
	static int compute(two_class_stats& st, const vector<double>& vd , const vector<double>& vp );
	static int compute(two_class_stats& st, const vector<bool>& vd , const vector<double>& vp );
	static int compute(two_class_stats& st, const vector<bool>& vd , const vector<bool>& vp );

	int compute(const vector<double>& vd , const vector<double>& vp ) {
		assert( vd.size() == vp.size() );
		reset();
		for(unsigned int i=0; i<vd.size(); ++i) incr( (vd[i]!=0.0), (vp[i]!=0.0) );
		return 1;
		}
		
	int compute(const vector<bool>& vd , const vector<double>& vp ) {
		assert( vd.size() == vp.size() );
		reset();
		for(unsigned int i=0; i<vd.size(); ++i) incr( vd[i], (vp[i]!=0.0) );
		return 1;
		}

	double fisher_pval() const { return ::fisher_exact_test(tp, fp, fn, tn) ; }
	
	bool operator<(const two_class_stats& s) const {
		if (tpr() != s.tpr()) return tpr() < s.tpr();
		if (fpr() != s.fpr()) return fpr() < s.fpr();
		return false;
		}
	};


vector<double> get_rank(const vector<double>& x) ;
double spearman(const vector<double>& x, const vector<double>& y, double* ppval=0) ;

#include <set>

template <typename T> vector<T> get_levels(const vector<T>& v) {
	set<T> vs;
	for(unsigned int i=0; i<v.size(); ++i) vs.insert( v[i] );
	vector<T> retval(vs.begin(), vs.end());
	
	return retval;
	}

template <typename T> double ranked_level(const vector<T>& v_uniq_sorted, T x) {
	if (v_uniq_sorted.size() <= 1 ) return 0.5 ;
	
	unsigned int i;
	for(i=0; i<v_uniq_sorted.size(); ++i) {
		if ( x <= v_uniq_sorted[i] ) break; 
		}
	if (i >= v_uniq_sorted.size() ) i = v_uniq_sorted.size() - 1;
	return double(i) / (v_uniq_sorted.size() - 1);
	}

	
template <typename T> double eucliean_distance(const vector<T> &v1, const vector<T> &v2) {
	
	assert( v1.size() == v2.size() );
	
	double sum_d2 = 0.0;
	for(unsigned int i=0;i<v1.size(); ++i ){
		double d = v1[i] - v2[i];
		sum_d2 += d * d;
		}
	return sqrt(sum_d2);
	}


#endif
