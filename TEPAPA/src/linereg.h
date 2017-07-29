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
#ifndef __linereg_h
#define __linereg_h 1

#include <math.h>
#include <stdio.h>
#include <vector>
// #include <set>

using namespace std;

typedef char point_label;

class point: public std::pair<double,double> {
	point_label _l;
	
	public:
	point(double x=NAN, double y=NAN, point_label pl=0):
		std::pair<double,double>(x,y)
		{
		_l = pl;
		}
	
	point_label l() const {return _l;}
	bool operator == (const point& p2) const
		{return first==p2.first && second==p2.second;}
	bool operator < (const point& p2) const
		{return first<p2.first;}
	};


// typedef std::set<point> points;
typedef std::vector<point> points;


class reg_line {
	double _m, _c, _r;
	
	public:
	reg_line(double pm=0, double pc=0, double pr=0)
		{
		_m = pm;
		_c = pc;
		_r = isnanf(pr) ? 1.00 : pr;
		}
	
	double operator() (double x) const {return _m * x + _c;}
	double m() const {return _m;}
	double c() const {return _c;}
	double r() const {return _r;}
	double r2() const {return _r * _r;}
	void print(FILE* fout=stdout) const;
	
// 	bool operator <(const reg_line& rl) const { r2() < rl.r2(); } 
	};


reg_line linear_regression(const points& data);
// reg_line linear_regression(const points_vec& data);



#endif // #ifndef __linereg_h
