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
#ifndef __stopwatch_h
#define __stopwatch_h 1

#include <time.h>
#include <string>

using namespace std;

class stopwatch {
	clock_t begin;
	
	public:
	stopwatch () { reset(); }
	void reset() { begin = 0; }
	void start() { begin = clock(); }
	double elapsed_sec() const { return double(clock() - begin) / CLOCKS_PER_SEC; }
	double eta_sec(double fraction) const { 
		double e = elapsed_sec() ;
		if ( fraction < 0 ) fraction = 1e-10;
		return ( e / fraction ) * (1 - fraction); 
		}
		
	std::string get_eta_str(double fraction) const  {
		char buf[80];
		double time_rem = eta_sec(fraction);
		struct tm * timeinfo;
		time_t now, eta;
		time (&now);
		eta = now + time_rem ;
		timeinfo = localtime (&eta);
		strftime (buf, 80, "%F %T", timeinfo);
		return string(buf);
		}
		
	std::string get_now_str() const  {
		char buf[80];
		struct tm * timeinfo;
		time_t now, eta;
		time (&now);
		eta = now ;
		timeinfo = localtime (&eta);
		strftime (buf, 80, "%F %T", timeinfo);
		return string(buf);
		}
		
	};

#endif // __stopwatch_h
