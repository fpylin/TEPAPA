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

#ifndef __tepapa_cooccurrence_h
#define __tepapa_cooccurrence_h 1

#include "tepapa-results.h"
#include "tepapa-gvar.h"
#include "ngram-pattern.h"
#include "tepapa-program.h"
#include "fgroup.h"


class TEPAPA_Program_Reduce_By_Cooccurrence: public TEPAPA_Program {
	protected: 
	bool f_subpattern_only;
	public:
	TEPAPA_Program_Reduce_By_Cooccurrence();

	virtual int run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) ;
	virtual TEPAPA_Program* spawn() const { return new TEPAPA_Program_Reduce_By_Cooccurrence; }
	};


#endif // __tepapa_cooccurrence_h
