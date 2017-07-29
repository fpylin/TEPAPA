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
///////////////////////////////////////////////////////////////////
#include "ecdf.h"
#include "stats.h"




ecdf::ecdf(const vector<double>& v): cdf() {
	for(unsigned int i=0; i<v.size(); ++i) {
		if (isnanf(v[i])) continue;
		data.push_back(v[i]);
		}
	sort(data.begin(), data.end());
	}
	
double ecdf::p(double x) {
	int i=0;
	int j=0;
	int n=data.size();
	for(i=0;   i<n;   ++i) if (x <  data[i]) break;
	for(j=n-1; j>=0;  --j) if (x >= data[j]) break;
// 	printf("%d\t%d\t%d\n", i, j, n);
// 	return double(i)/double(n); // 
// 	return double(i+0.5)/double(n+1); // 
	double frac = 0.0;
// 	if ( (i==j) && (i!=(n-1)) ) {
// 		double l = (data[i+1] - data[i]);
// 		double d = (x - data[i]);
// 		frac = d / l;
// 		}
	return double(i+j+(frac*2)+1)/double(n+2)/2.0; // 
// 	return double(i)/double(n);
	}

	
//////////////////////////////////////////////////////////////////////////

kernel_density_estimator::kernel_density_estimator (const vector<double>& v, double h)
	: cdf() {
	for(unsigned int i=0; i<v.size(); ++i) {
		if (isnanf(v[i])) continue;
		data.push_back(v[i]);
		}
	sort(data.begin(), data.end());
	
	if (!isnanf(h)) 
		set_eval_points(h); 
	else 
		set_eval_points(estimate_bw());
	}
	
// 	virtual double kernel(double u) = 0 ; 
// triangular 
// 	inline double kernel(double u) { return fabs( u ) < 1 ? 0.5 : 0; }// uniform
// 	inline double kernel(double u) { // gaussian
// 		return 1 / sqrt( 2 * M_PI) * exp ( - 0.5 * u * u ) ;
// 		}// uniform

	
void kernel_density_estimator::set_eval_points(double h) {
	bw = h;
	eval_points.clear();
	for(unsigned int i=0; i<data.size(); ++i) {
		for (int j=-1; j<=1; ++j) {
			eval_points.insert( data[i] + h * j );
// 			eval_points.insert( data[i] );
// 			eval_points.insert( data[i] + h * j / 20 );
			}
		}
	}
	
double kernel_density_estimator::kernel_density(double x) {
	double s = 0;
	if ( kde_cache.find(x) != kde_cache.end() ) return kde_cache[x] ;
	for(unsigned int i=0; i<data.size(); ++i) s += kernel( (x - data[i]) / bw );
	s /= ( double(data.size()) * bw );
	kde_cache[x] = s;
	return s;
	}
	
double kernel_density_estimator::p(double x) {
	set<double> ep = eval_points;
	ep.insert(x);
	set<double>::iterator epi=ep.begin(); 
	double x0 = *epi;
	double y0 = kernel_density(x0);
	double auc = 0.0;
	++epi;
	while( epi!=ep.end() && *epi <= x )  {
		double x1 = *epi;
		double y1 = kernel_density(x1);
		auc += (x1 - x0) * (y0 + y1) / 2.0;
		x0 = x1;
		y0 = y1;
		++epi;
		}
	if ( auc < 0.0 ) auc = 0.0;
	if ( auc > 1.0 ) auc = 1.0;
	return auc;
	}

double kernel_density_estimator::estimate_bw() const {
	double sigma = sd(data);
	double iqr = +INFINITY;
	double n = data.size();
	double hm = 0.9 * min(sigma, iqr*1.34 )* pow(n, 0.2);
	hm /= sqrt( 2 * M_PI );
	return hm;
	}


///////////////////////////////////////////////////////////////////////////////////////////////////

void gaussian_kde::set_eval_points(double h) {
	bw = h;
	}

///////////////////////////////////////////////////////////////////////////////////////////////////
double gaussian_kde::p(double x) {
	double retval = 0.0;
// 	printf("bw = %f\n", bw);
	for(unsigned int i=0; i<data.size(); ++i) {
		double s = data[i];
// 		printf("  data[%d] = %f\t", i, s);
		double a = 0.5 * ( 1 + erf( (x - s) / (sqrt(2) * bw ) ) );
// 		printf("  a = %f\t",  a);
		retval += a;
// 		printf("  retval = %f\n", retval);
		}

	retval /= double( data.size() );
// 	printf("  final retval = %f\n", retval);
	return retval ;
	}
	
// 
