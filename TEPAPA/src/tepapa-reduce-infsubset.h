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

#ifndef __tepapa_infsubset_h
#define __tepapa_infsubset_h 1

#include "tepapa-results.h"
#include "tepapa-gvar.h"
#include "ngram-pattern.h"
#include "tepapa-program.h"


class TEPAPA_Program_Reduce_Inferior_Subsets: public TEPAPA_Program {
	double f_collapsing_by_signature;
	double f_embed_subsets;
	protected: 

	public:
	TEPAPA_Program_Reduce_Inferior_Subsets();

	virtual int run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) ;
	virtual TEPAPA_Program* spawn() const { return new TEPAPA_Program_Reduce_Inferior_Subsets; }
	};


#endif // __tepapa_infsubset_h
