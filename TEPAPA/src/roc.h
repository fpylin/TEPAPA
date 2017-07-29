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
#ifndef __roc_h
#define __roc_h 1

#include "stats.h"

using namespace std;

#include <set>
#include <vector>

struct two_class_stats_with_threshold: public two_class_stats
	{
	double threshold;
	
	two_class_stats_with_threshold(): two_class_stats() 
		{
		threshold=0;
		}
	
	two_class_stats_with_threshold(const two_class_stats_with_threshold& s): two_class_stats(s)
		{
		threshold=s.threshold;
		}
	
	two_class_stats_with_threshold(const two_class_stats& s): two_class_stats(s) 
		{
		threshold=0;
		}
	
	bool operator<(const two_class_stats_with_threshold& t) const  // for sorting 
		{
		return two_class_stats::operator<(t);
		}
	};



struct pc_pair 
	{
	double p;
	bool   c;
	
	pc_pair(double pp = 0.0, double pc = 0.0) { p = pp; c = (pc != 0.0); }
	pc_pair(double pp = 0.0, bool pc = false) { p = pp; c = pc; }

	bool operator < (const pc_pair& pcp) const  { return p < pcp.p; }
	};

	

class roc_curve: public set<two_class_stats_with_threshold>
	{
	static two_class_stats_with_threshold   dummy;
	vector<two_class_stats_with_threshold>  threshold_stats;
	
	void compute_roc_curve_proper(vector<pc_pair>& data);
	
	public:
	roc_curve(): set<two_class_stats_with_threshold>  () {}
	
	
	roc_curve( const vector<double>& v_cond, const vector<double>& v_pred)
		: set<two_class_stats_with_threshold>  ()
		{
		compute( v_cond, v_pred );
		}
		

	void compute( const vector<double>& v_cond, const vector<double>& v_pred);
	void compute( const vector<bool>& v_cond, const vector<double>& v_pred);
	
	const two_class_stats_with_threshold&  find_threshold(double tau);
	
	double AUC (double* pvalue=0) const;
	double AUC (double& ci95l, double& ci95h) const;
	static double SE_AUC(double auc, double na, double nn);
	
	double optimal_cutoff(double& max_Hm, double& max_Hm_sens, double& max_Hm_spec, double factor=1.0) const;
	double optimal_cutoff(bool reversed=false) const;
	
	double at_spec(double x, bool reversed=false, double* corr_eauc=0) const;
	double get_P_by_MWWUT() const;
	
	virtual void print_thresholds(const string& fmt="HFefghabcdlm", FILE* file=stdout) const;
	};


#include "ecdf.h"

class roc_curve_kernel: public roc_curve {
// 	vector<double> posex, negex; 
	iptr<kernel_density_estimator> posex, negex;
	double prevalence; 
	
	void calculate_kernel_densities( const vector<double>& v_cond, const vector<double>& v_pred, double bw_frac) ;
	
	public:
		
	roc_curve_kernel( const vector<double>& v_cond, const vector<double>& v_pred, double bw_frac=10) :
		roc_curve(v_cond, v_pred) 
		{
		calculate_kernel_densities(v_cond, v_pred, bw_frac);
		}

	double sens(double x) { return 1.0 - posex->p(x); }
	double spec(double x) { return negex->p(x); }
	
	virtual void print_thresholds_2(const string& fmt="HFefghabcdlm", FILE* file=stdout) ;
	};


void print_roc_stats (
	const vector<double>& v_cond,
	const vector<double>& v_pred
	);


#define DEBUG 0

#endif

