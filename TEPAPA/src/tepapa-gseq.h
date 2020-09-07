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

#ifndef __tepapa_gseq_h
#define __tepapa_gseq_h 1

#include "hashtable.h"
#include "ngram-pattern.h"
#include "ngram-registry.h"
#include "samples.h"

#include "tepapa-discoverer.h"
#include "tepapa-evaluator.h"
#include "tepapa-gvar.h"

#include "tepapa-results.h"
#include "tepapa-program.h"

#include "gseq-pattern.h"

using namespace std;

#include <vector>
#include <set>

extern set<hash_value> separators;


#include "tepapa-program.h"


class TEPAPA_Gapped_Sequence_Discoverer: public TEPAPA_Discoverer {
  int traverse_gseq_btree(TEPAPA_Evaluator& e, 
			  const gapped_sequence_pattern& gs, size_t index,
			  const binary_profile& mmask,  const matching_range_list& mrlist,
			  int depth=10, int wanted_dir=0) ;

 protected:
  virtual void run1(int i) {};

 public:

 TEPAPA_Gapped_Sequence_Discoverer(): TEPAPA_Discoverer() {}
 TEPAPA_Gapped_Sequence_Discoverer(VariableList& param_ref): TEPAPA_Discoverer(param_ref) {}

  virtual ~TEPAPA_Gapped_Sequence_Discoverer() {}


  virtual bool run() ;  // de novo learning
} ;




class TEPAPA_Program_Discoverer_Gapped_Sequence: public TEPAPA_Program_Discoverer<TEPAPA_Gapped_Sequence_Discoverer> {

 protected: 
  // 	virtual TEPAPA_Discoverer*  create_instance() { return new TEPAPA_Discoverer_Gapped_Sequence; }

 public:
  TEPAPA_Program_Discoverer_Gapped_Sequence();

  virtual TEPAPA_Program* spawn() const { return new TEPAPA_Program_Discoverer_Gapped_Sequence; }
};

	
#endif // __tepapa_gseq_h
