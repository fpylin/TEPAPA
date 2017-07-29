/* align.cc
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

#include <stdio.h>
#include <ctype.h>

// bool nw_eq(const char& c1, const char& c2)
// 	{ 
// 	return tolower(c1) == tolower(c2);
// 	}

// #include "align.h"


// int main(void)
// 	{
// 	vector<char> vc1 = to_vector( "ABCDFGTATAISTA" );
// 	vector<char> vc2 = to_vector( "ABCEFHTAISTA" );
// 	
// 	needleman_wunsch<char> nw1(1, 0, 0);
// 	needleman_wunsch<char> nw2(5, -4, -1);
// 	smith_waterman<char>   sw1(5, -4, -1);
// 	
// 	nw1(vc1, vc2).print(vc1,vc2); putchar('\n');
// 	nw2(vc1, vc2).print(vc1,vc2); putchar('\n');
// 	sw1(vc1, vc2).print(vc1,vc2); putchar('\n');
// 	
// 	sw1(vc1, vc2).print(vc1,vc2, "-","#####", false, "\n", "\t-------\t"); putchar('\n');
// 
// 	return 0;
// 	}
