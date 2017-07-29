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
#include <math.h>
#include "linereg.h"
 
 
void reg_line::print(FILE* fout) const {
	fprintf(fout, "y = %.4f * x + %.4f", _m, _c);
	}
	

reg_line linear_regression(const points& data) {
	double n      = data.size();
	double sum_xy = 0.0;
	double sum_x  = 0.0;
	double sum_x2 = 0.0;
	double sum_y  = 0.0;
	double sum_y2 = 0.0;
	
	points::const_iterator i;
	points::const_iterator e=data.end();
	
	for(i=data.begin(); i!=e; ++i)
		{
		register double x = i->first;
		register double y = i->second;
		sum_xy += x * y;
		sum_x += x;
		sum_x2 += x * x;
		sum_y += y;
		sum_y2 += y * y;
		}
	
	double te = (n * sum_xy - sum_x * sum_y);
	double xe = (n * sum_x2 - sum_x * sum_x);
	double ye = (n * sum_y2 - sum_y * sum_y);
	if ( (xe == 0.0) || (ye == 0.0) ) { return reg_line(); }
	double m = te / xe; // slope
	double c = (sum_y - m * sum_x) / n; // intercept
	double r = te / sqrt(xe * ye); // correlation coefficient
	
	return reg_line(m, c, r);
	}


// reg_line linear_regression(const points_vec& data) {
// 	points p;
// 	for (points_vec::const_iterator i=data.begin(); i!=data.end(); ++i) {
// 		p.insert(*i);
// 		}
// 	return linear_regression(p);
// 	}
// 
