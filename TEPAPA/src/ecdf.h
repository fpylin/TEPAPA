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

#ifndef __ecdf_h
#define __ecdf_h 1

#include <math.h>

#include <vector>
#include <algorithm>
#include <map>
#include <set>


using namespace std;

#include "iptr.h"

inline double estimate_num_precision(const vector<double>& v) {
	set<double> vs;
	for(unsigned int i=0; i<v.size(); ++i) vs.insert(v[i]);
	double sum_v_delta = 0.0;
	vector<double> w;
	for(set<double>::iterator i=vs.begin(); i!=vs.end(); ++i) w.push_back(*i);
	for(unsigned int j=0; j<w.size()-1; ++j) sum_v_delta += w[j+1] - w[j] ;
	return sum_v_delta / w.size();
	}

inline double estimate_num_precision(const vector<double>& v1, const vector<double>& v2) {
	set<double> vs;
	for(unsigned int i=0; i<v1.size(); ++i) vs.insert(v1[i]);
	for(unsigned int i=0; i<v2.size(); ++i) vs.insert(v2[i]);
	double sum_v_delta = 0.0;
	vector<double> w;
	for(set<double>::iterator i=vs.begin(); i!=vs.end(); ++i) w.push_back(*i);
	for(unsigned int j=0; j<w.size()-1; ++j) sum_v_delta += w[j+1] - w[j] ;
	return sum_v_delta / w.size();
	}

///////////////////////////////////////////////////////////////////
class cdf: public __iptr_member {
	public: 
	cdf(): __iptr_member() {}
	virtual ~cdf() {}
	virtual double p(double x) =0;
	virtual const vector<double>& thresholds(void) =0;
	};

///////////////////////////////////////////////////////////////////
class ecdf: public cdf {
	protected:
	vector<double> data;
	
	public:
	ecdf(const vector<double>& v) ;
	virtual ~ecdf() {}
	virtual double p(double x) ;
	virtual const vector<double>& thresholds(void) {return data;}
	};


//////////////////////////////////////////////////////////////////////////
class kernel_density_estimator: public cdf  {
	protected:
	vector<double> data;
	set<double> eval_points;
	map<double, double> kde_cache;
	double bw; 
	
	public:
	kernel_density_estimator (const vector<double>& v, double h=NAN) ;
	virtual ~kernel_density_estimator (){} 
	
	virtual double estimate_bw() const;
	
	virtual double kernel(double u) = 0 ; 
// triangular 
// 	inline double kernel(double u) { return fabs( u ) < 1 ? 0.5 : 0; }// uniform
// 	inline double kernel(double u) { // gaussian

// 		}// uniform

	virtual void set_eval_points(double h) ;
	
	virtual double kernel_density(double x) ;
	
	virtual double p(double x) ;
	
	virtual const vector<double>& thresholds(void) {return data;}
	};


//////////////////////////////////////////////////////////////////////////
class triangular_kde: public kernel_density_estimator {
	public:
	triangular_kde(const vector<double>& v, double h=NAN): kernel_density_estimator(v,h) {}
	
	// triangular 
	virtual double kernel(double u) { double v = 1 - fabs( double(u) ); return v > 0 ? v : 0; } 
	};


//////////////////////////////////////////////////////////////////////////
class gaussian_kde: public kernel_density_estimator {
	public:
	gaussian_kde(const vector<double>& v, double h=NAN): kernel_density_estimator(v,h) {}
	
	virtual double kernel(double u) { return 1 / sqrt( 2 * M_PI) * exp ( - 0.5 * u * u ) ; } 
	
	virtual void set_eval_points(double h) ;
	
	virtual double p(double x) ;
	};



#endif // #ifndef __ecdf_h
