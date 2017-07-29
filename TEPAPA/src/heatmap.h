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

#ifndef __heatmap_h
#define __heatmap_h 1 

#include "tepapa-dataset.h"
#include "bhc.h"

class heatmap {
	TEPAPA_dataset  ds;
	
	double dump_dendrogram_r(string& out, const BHC_node* root, 
		const TEPAPA_Results& patterns, vector<int>& reorder, 
		double& diff_max, double nearest_diff, int delta_start
		) const;

	double dump_dendrogram_r(string& out, const BHC_node* root, 
		const feature_arrays& patterns, vector<int>& reorder, 
		double& diff_max, double nearest_diff, int delta_start
		) const;

	template <class T> bool dump_dendrogram(string& out, BHC_Tree& bht, const T& vr, double& diff_max, vector<int>& reorder, int delta_start=0) {
		BHC_node* root = bht.get_root();
		
		dump_dendrogram_r(out, root, vr, reorder, diff_max, root->diff, delta_start);
		
		return true;
		}
	
	public:
	heatmap(const TEPAPA_dataset& p_ds) {
		ds = p_ds;
		}
	
	bool dump(string& out, BHC_Tree& bht_features) ;
	};

#endif // __heatmap_h
