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

#ifndef __bhc_h
#define __bhc_h 1

using namespace std;

#include <vector>
#include <string>
#include <future>

#include "parallel.h"
#include "matrix.h"
#include "bprof.h"
#include "strfriends.h"

#include "tepapa-results.h"
#include "tepapa-dataset.h"


class BHC_node {
	private:
// 	vector<int> traverse_cache;
// 	std::mutex  traverse_cache_mutex;
	
	void traverse(vector<int>& out) ;
	
	public:
	int     id;
	double  diff;
	
	BHC_node* left;
	BHC_node* right;
	BHC_node() { left=0; right=0; id=-1; diff=0.0; }
	BHC_node(BHC_node* pleft, BHC_node* pright, int pid, double pdiff) {
		left = pleft;
		right = pright;
		id = pid;
		diff = pdiff;
		}
	
	~BHC_node() { 
		if (left) delete left;
		if (right) delete right;
		}
	
	vector<int> traverse() {
		vector<int> retval;
		traverse(retval);
		return retval;
		}
	};

void traverse_BHC(vector<int>& out, const BHC_node* root) ;
vector<int> traverse_BHC(const BHC_node* root) ;
double mean_diff(const mat& simmat, const BHC_node* rootsi, const BHC_node* rootsj);

#define BHC_DISTANCE_METRIC_EUCLIDEAN    1
#define BHC_DISTANCE_METRIC_FISHEREXACT  2
#define BHC_DISTANCE_METRIC_MANHATTAN    3

#define BHC_LINKAGE_CRITERIA_COMPLETE    1
#define BHC_LINKAGE_CRITERIA_SINGLE      2
#define BHC_LINKAGE_CRITERIA_MEAN        3
#define BHC_LINKAGE_CRITERIA_CENTROID    4

void load_simmat1(mat& simmat, const TEPAPA_Results& elements, int distance_metric, int i, int start, int end) ;

void load_simmat1(mat& simmat, const feature_arrays& elements, int distance_metric, int i, int start, int end) ;

string BHC_Tree_feature_name(const TEPAPA_Result& r) ;

string BHC_Tree_feature_name(const feature_array& s) ;


class BHC_Tree {
	BHC_node* root;

	void do_hierarchical_clustering_find_min_d(const vector<BHC_node*> roots, const mat& simmat, int start, int i, int linkage_criteria, double& min_d, int& min_at_i, int& min_at_j) ;
	
	template <typename T> BHC_node* do_hierarchical_clustering(const T& elements, int distance_metric, int linkage_criteria, int start=0, int end=-1) ;
	
	template <typename T> int dump_bhtree_r(const BHC_node* root, const T& patterns, const string& prefix = "") const;
	
	void get_thresholds_r(vector<double>& out, const BHC_node* r) const;
	void do_cluster_r(vector< vector<int> >& out, BHC_node* root, double thres);
	
	public:
	BHC_Tree() { root = 0; }
	
	virtual ~BHC_Tree() { if (root) delete root; }
	
	BHC_node* get_root() const { return root; }
	
	bool operator !() const { return !root; }
	
	template <typename T> bool gentree(const T& elements, int distance_metric, int linkage_criteria, int start=0, int end=-1) {
		if (root) delete root;
		if (end == -1) end = elements.size();
		
		root = do_hierarchical_clustering(elements, distance_metric, linkage_criteria, start, end);
		
		return !!root;
		}

	template <typename T> BHC_Tree(const T& elements, int distance_metric, int linkage_criteria) { 
		root = 0; 
		int start = 0;
		int end = elements.size();
		
		root = do_hierarchical_clustering(elements, distance_metric, linkage_criteria, start, end);
		}

	template <typename T> int dump(const T& patterns, const string& prefix = "") const {
		return dump_bhtree_r( root, patterns, prefix);
		}
		
	vector<double> get_thresholds() const ;
	
	vector< vector<int> >  do_cluster(double thres) ;
	};



//////////////////////////////////////////////////////////////////////////////
// #define TEPAPA_MULTITHREAD 0
	
template <typename T> mat load_simmat(const T& elements, int distance_metric, int start, int end) {
	int dim = end - start ; // elements.size();
	
	mat simmat(dim, dim);
	
	if (dim<50)  {
		for(int i=start; i<end; ++i) load_simmat1(simmat, elements, distance_metric, i, start, end);
		return simmat;
		}
#ifdef TEPAPA_MULTITHREAD
		thread_manager tm;
#endif // TEPAPA_MULTITHREAD
	
	for(int i=start; i<end; ++i) {
#ifdef TEPAPA_MULTITHREAD
		tm.queue();
		tm.push_back(
			std::async( std::launch::async, 
				[distance_metric,i,start,end,&simmat,&elements]{ load_simmat1(simmat, elements, distance_metric, i, start, end); }
				)
			);
#else 
		load_simmat1(simmat, elements, distance_metric, i, start, end);
#endif // TEPAPA_MULTITHREAD
		}
	
#ifdef TEPAPA_MULTITHREAD
		tm.join_all();
#endif // TEPAPA_MULTITHREAD
	
	return simmat;
	}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// double sum (const vector<double>& v) ;
int compare_branches(const TEPAPA_Results& elements, BHC_node* root1, BHC_node* root2) ;
int compare_branches(const feature_arrays& elements, BHC_node* root1, BHC_node* root2) ;


template <typename T> BHC_node* BHC_Tree::do_hierarchical_clustering(const T& elements, int distance_metric, int linkage_criteria, int start, int end) {
	
// 	unsigned int dim = elements.size();
	unsigned int dim = end - start;
	
	msgf(VL_INFO, "Calculating similarity matrix (%u x %u).\n", dim, dim );
	
	assert( end - start > 1 );
	
	mat simmat = load_simmat(elements, distance_metric, start, end);
	
// 	assert ( (simmat.r() == dim) && (simmat.c() == dim) );
	
	msgf(VL_INFO, "Loading roots (dim=%d).\n", dim);
	
	vector<BHC_node*>  roots(dim);
	
	for(int i=start; i<end; ++i) roots[i-start] = new BHC_node(0, 0, i, 0.0);
	
	msgf(VL_INFO, "Do hierarchical clustering.\n");

#ifdef TEPAPA_MULTITHREAD
		thread_manager tm;
#endif // TEPAPA_MULTITHREAD
	
	while(roots.size() > 1) {
		int min_at_i = 0, min_at_j = 0;
		double min_d = 9e99;
		
		for(unsigned int i=0; i<roots.size(); ++i) { // find out the position of the smallest difference;
#ifdef TEPAPA_MULTITHREAD
			tm.queue();
			tm.push_back(
				std::async( std::launch::async, 
					&BHC_Tree::do_hierarchical_clustering_find_min_d,
					this, std::ref(roots), std::ref(simmat), start, i, linkage_criteria, 
					std::ref(min_d), std::ref(min_at_i), std::ref(min_at_j)
					)
				);
#else 
			do_hierarchical_clustering_find_min_d(roots, simmat, start, i, linkage_criteria, min_d, min_at_i, min_at_j) ;
#endif // TEPAPA_MULTITHREAD
			}
		
#ifdef TEPAPA_MULTITHREAD
		tm.join_all();
#endif // TEPAPA_MULTITHREAD
		

		int d = compare_branches( elements, roots[min_at_i], roots[min_at_j] ) ;
		
		// then merge it
		BHC_node* new_node ;
		if (d < 0) {
			new_node = new BHC_node(roots[min_at_i], roots[min_at_j], -1, min_d) ;
			}
		else {
			new_node = new BHC_node(roots[min_at_j], roots[min_at_i], -1, min_d) ;
			}
		
		roots.erase( roots.begin() + min_at_j );
		roots.erase( roots.begin() + min_at_i );
		roots.push_back(new_node);
		}

	return roots[0];
	}


template <typename T> int BHC_Tree::dump_bhtree_r(const BHC_node* root, const T& patterns, const string& prefix) const {
	string s = prefix ;
	string out = strfmt("%s", prefix.c_str(), root->id);
	if (root->id < 0) {
		out += strfmt("\\ (%.3f)", root->diff);
		}
	else {
		out = strfmt("%-25s [%2d] ", out.c_str(), root->id);
		out += BHC_Tree_feature_name( patterns[ root->id ] );
		}
// 	out += string("\n");
	printf( "%s\n", out.c_str() );
	
	s += "|";
	if (root->left) dump_bhtree_r(root->left, patterns, s);
	if (root->right) dump_bhtree_r(root->right, patterns, s);
	return 1;
	}


	
#endif // __bhc_h
