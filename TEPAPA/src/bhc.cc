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

#include "config.h"
#include "bhc.h"
#include "tepapa-msg.h"
#include "stats.h"
#include <math.h>

void BHC_node::traverse(vector<int>& out) {
// #ifdef TEPAPA_MULTITHREAD
// 	traverse_cache_mutex.lock();
// #endif // TEPAPA_MULTITHREAD
// 	if ( traverse_cache.size() ) { 
// 		out = traverse_cache;
// 		out.insert( out.end(), traverse_cache.begin(), traverse_cache.end() );
// 		}
// 	else {
// #ifdef TEPAPA_MULTITHREAD
// 		traverse_cache_mutex.unlock();
// #endif // TEPAPA_MULTITHREAD
		if(id>=0) out.push_back(id);
		if(left)  left->traverse(out);
		if(right) right->traverse(out);
// #ifdef TEPAPA_MULTITHREAD
// 		traverse_cache_mutex.lock();
// #endif // TEPAPA_MULTITHREAD
// 		
// // 		traverse_cache=out;
// 		}
// #ifdef TEPAPA_MULTITHREAD
// 		traverse_cache_mutex.unlock();
// #endif // TEPAPA_MULTITHREAD		
	}


double mean_diff(const mat& simmat, const vector<int>& is, const vector<int>& js) {
	
	
	double sum = 0;
	int n=0;
	for(unsigned int i=0; i<is.size(); ++i) {
		for(unsigned int j=0; j<js.size(); ++j) {
			double d = simmat(is[i], js[j]);
			sum += d;
// 			printf("\t%d,%d = %f\n", is[i], js[j], d);
			++n;
			}
		}
	return sum/double(n);
	}



vector<double> get_centroid(const TEPAPA_Results& elements, const vector<int>& indexes) {
	vector<double> centroid;
	for(unsigned int i=0; i<indexes.size(); ++i) {
		int ii = indexes[i];
		if (! centroid.size() ) {
			for (unsigned int k=0; k<elements[ii].bprof.size(); ++k) {
				centroid.push_back( elements[ii].bprof[k] ? 1 : 0 );
				}
			}
		else {
			for (unsigned int k=0; k<elements[ii].bprof.size(); ++k) {
				centroid[k] += ( elements[ii].bprof[k] ? 1 : 0 );
				}
			}
		}
	for (unsigned int k=0; k<centroid.size(); ++k) {
		centroid[k] /= double( indexes.size() );
		}
	return centroid;
	}


double centroid_diff(const TEPAPA_Results& elements, const vector<int>& is, const vector<int>& js) {
	vector<double> rootsi_centroid = get_centroid(elements, is);
	vector<double> rootsj_centroid = get_centroid(elements, js);
	
	assert( rootsi_centroid.size() == rootsj_centroid.size() );
	
	return eucliean_distance(rootsi_centroid, rootsj_centroid);
	}

double max_diff(const mat& simmat, const vector<int>& is, const vector<int>& js) {
	double maxd = 0;
	
// 	printf("%d,%d:\n", is.size(), js.size());
// 	for(int i=0; i<is.size(); ++i) printf("%d ", is[i]); printf("\n");
// 	for(int j=0; j<js.size(); ++j) printf("%d ", js[j]); printf("\n");
	
// 	printf("%d\t%d\t%d\t%d\n", is.size(), js.size(), simmat.r(), simmat.c());
	for(unsigned int i=0; i<is.size(); ++i) {
		for(unsigned int j=0; j<js.size(); ++j) {
			double d = simmat(is[i], js[j]);
			if (maxd < d) maxd = d;
			}
		}
	return maxd;
	}

double min_diff(const mat& simmat, const vector<int>& is, const vector<int>& js) {
	double mind = 9e99;
	for(unsigned int i=0; i<is.size(); ++i) {
		for(unsigned int j=0; j<js.size(); ++j) {
			double d = simmat(is[i], js[j]);
			if (mind > d) mind = d;
			}
		}
	return mind;
	}
	



#include "strfriends.h"


void load_simmat1(mat& simmat, const TEPAPA_Results& elements, int distance_metric, int i, int start, int end) {
// 	int dim = elements.size();

// 	printf("(%d, %d) -> (%d, %d)\n", i, i, i-start, i-start);
	simmat(i-start, i-start) = 0;

	for(int j=i+1; j<end; ++j) {
		double d = 0;
		const binary_profile& bp1 = elements[i].bprof;
		const binary_profile& bp2 = elements[j].bprof;
		bool f_bp1_is_numeric = elements[i].is_numeric();
		bool f_bp2_is_numeric = elements[j].is_numeric();
		bool f_is_numeric = ( f_bp1_is_numeric | f_bp2_is_numeric );
		
		if (f_is_numeric) {
			binary_profile bpc = bp1 ;
			bpc &= bp2;
			vector<double> vp1 = subset(elements[i].val, bpc);
			vector<double> vp2 = subset(elements[j].val, bpc);
			
			switch(distance_metric) {
				case BHC_DISTANCE_METRIC_EUCLIDEAN:   d = eucliean_distance(vp1, vp2); break;
// 				case BHC_DISTANCE_METRIC_MANHATTAN:   d = normalised_distance(vp1, vp2)); break;
				default: msgf(VL_FATAL, "Invalid BHC distance metric."); break;
				};
			}
		else { // binary
			switch(distance_metric) {
				case BHC_DISTANCE_METRIC_EUCLIDEAN:   d = (double)(binary_profile::eucliean_distance(bp1, bp2)); break;
				case BHC_DISTANCE_METRIC_MANHATTAN:   d = (double)(binary_profile::normalised_distance(bp1, bp2)); break;
				case BHC_DISTANCE_METRIC_FISHEREXACT: d = (double)(binary_profile::fisher_exact_pval(bp1, bp2));  break;
				default: msgf(VL_FATAL, "Invalid BHC distance metric."); break;
				};
			}
		simmat(i-start, j-start) = d;
		simmat(j-start, i-start) = d;
		}
	}

void load_simmat1(mat& simmat, const feature_arrays& elements, int distance_metric, int i, int start, int end) {
// 	int dim = elements.size();

// 	printf("(%d, %d) -> (%d, %d)\n", i, i, i-start, i-start);
	simmat(i-start, i-start) = 0;

	bool f_is_numeric = elements.has_numeric_features();
	
	const feature_array& fa1 = elements[i] ;

	for( int j=i+1; j<end; ++j ) {
		double d = 0;
		
		const feature_array& fa2 = elements[j];
		
		if (f_is_numeric) {
			binary_profile nm1 = ( ! fa1.mask_numeric ); nm1 |= fa1.bp ;
			binary_profile nm2 = ( ! fa2.mask_numeric ); nm2 |= fa2.bp ;
			binary_profile bpc = nm1 ;  bpc &= nm2 ;
			
			vector<double> vp1 = subset(fa1.vp, bpc);
			vector<double> vp2 = subset(fa2.vp, bpc);
			
			switch(distance_metric) {
				case BHC_DISTANCE_METRIC_EUCLIDEAN:   d = eucliean_distance(vp1, vp2); break;
// 				case BHC_DISTANCE_METRIC_MANHATTAN:   d = normalised_distance(vp1, vp2)); break;
				default: msgf(VL_FATAL, "Invalid BHC distance metric."); break;
				};
			}
		else { // binary
			switch(distance_metric) {
				case BHC_DISTANCE_METRIC_EUCLIDEAN:   d = (double)(binary_profile::eucliean_distance(fa1.bp, fa2.bp)); break;
				case BHC_DISTANCE_METRIC_MANHATTAN:   d = (double)(binary_profile::normalised_distance(fa1.bp, fa2.bp)); break;
				case BHC_DISTANCE_METRIC_FISHEREXACT: d = (double)(binary_profile::fisher_exact_pval(fa1.bp, fa2.bp));  break;
				default: msgf(VL_FATAL, "Invalid BHC distance metric."); break;
				};
			}
		simmat(i-start, j-start) = d;
		simmat(j-start, i-start) = d;
		}	
	}

	



void BHC_Tree::do_hierarchical_clustering_find_min_d(const vector<BHC_node*> roots, const mat& simmat, int start, int i, int linkage_criteria, double& min_d, int& min_at_i, int& min_at_j) {
	
	std:: mutex  do_hierarchical_clustering_find_min_d_mutex;

	vector<int> is = roots[i]->traverse();
	for(unsigned int i=0; i<is.size(); ++i) { is[i] -= start; assert(is[i]>=0); }
	
	for(unsigned int j=i+1; j<roots.size(); ++j) {
		double d = 0;
		vector<int> js = roots[j]->traverse();
		
		for(unsigned int j=0; j<js.size(); ++j) { js[j] -= start; assert(js[j]>=0); }
		
		switch(linkage_criteria) {
			case BHC_LINKAGE_CRITERIA_COMPLETE: d = max_diff(  simmat, is, js ) ;  break;
			case BHC_LINKAGE_CRITERIA_SINGLE:   d = min_diff(  simmat, is, js ) ;  break;
			case BHC_LINKAGE_CRITERIA_MEAN:     d = mean_diff( simmat, is, js ) ;  break;
// 			case BHC_LINKAGE_CRITERIA_CENTROID: d = centroid_diff( elements, is, js) ;  break;
			default: msgf(VL_FATAL, "Invalid BHC linkage criteria."); break;
			};

#ifdef TEPAPA_MULTITHREAD
		do_hierarchical_clustering_find_min_d_mutex.lock();
#endif // TEPAPA_MULTITHREAD
		if ( d < min_d ) {
			min_d = d; 
			min_at_i = i; 
			min_at_j = j;
			}
#ifdef TEPAPA_MULTITHREAD
		do_hierarchical_clustering_find_min_d_mutex.unlock();
#endif // TEPAPA_MULTITHREAD
		}
	}


string BHC_Tree_feature_name(const TEPAPA_Result& r) {
	return strfmt("%9.4g %s %s", r.pval, r.bprof.digest().c_str(), r.patt->to_string().c_str() );
	}

string BHC_Tree_feature_name(const feature_array& s) {
	return strfmt("%9.4g %s", s.score, s.name_str.c_str() );
	}



	

	
void BHC_Tree::get_thresholds_r(vector<double>& out, const BHC_node* r) const {
	out.push_back(r->diff);
	if (r->left) get_thresholds_r(out, r->left);
	if (r->right) get_thresholds_r(out, r->right);
	}

vector<double> BHC_Tree::get_thresholds() const {
	vector<double> retval;
	get_thresholds_r(retval, root);
	sort( retval.rbegin(), retval.rend() );
	return retval;
	}


void BHC_Tree::do_cluster_r(vector< vector<int> >& out, BHC_node* root, double thres) {
	if (root->diff > thres) {
		if (root->left) do_cluster_r(out, root->left, thres);
		if (root->right) do_cluster_r(out, root->right, thres);
		}
	else { // root->diff <= thres
		vector<int> grouped = root->traverse();
		out.push_back(grouped);
		}
	}
	
vector< vector<int> >  BHC_Tree::do_cluster(double thres) {
	vector< vector<int> >  retval;
	do_cluster_r(retval, root, thres);
	return retval;
	}


double sum (const vector<double>& v) {
	double s = 0.0;
	for(unsigned int i=0; i<v.size(); ++i) s += v[i];
	return s ;
	}


int compare_branches(const TEPAPA_Results& elements, BHC_node* root1, BHC_node* root2) {
	
	vector<int> nodes1 = root1->traverse();
	vector<int> nodes2 = root2->traverse();
	
// 	double z1 = 1.00; 	for(unsigned int i=0; i<nodes1.size(); ++i) z1 = min(z1, elements[ nodes1[i] ].pval);
// 	double z2 = 1.00; 	for(unsigned int j=0; j<nodes2.size(); ++j) z2 = min(z2, elements[ nodes2[j] ].pval);
// 	
// 	if (z1 < z2) return -1;
// 	if (z1 > z2) return  1;
	
	double z1, z2;
	z1 = 1.00; for(unsigned int i=0; i<nodes1.size(); ++i) z1 = min(z1, elements[ nodes1[i] ].pval);
	z2 = 1.00; for(unsigned int j=0; j<nodes2.size(); ++j) z2 = min(z2, elements[ nodes2[j] ].pval);
	
	if (z1 < z2) return -1;
	if (z1 > z2) return +1;

	z1 = -9e99; for(unsigned int i=0; i<nodes1.size(); ++i) z1 = max(z1, elements[ nodes1[i] ].est);
	z2 = -9e99; for(unsigned int j=0; j<nodes2.size(); ++j) z2 = max(z2, elements[ nodes2[j] ].est);
	
	if (z1 < z2) return +1;
	if (z1 > z2) return -1;
	
	return 0;
	}

int compare_branches(const feature_arrays& elements, BHC_node* root1, BHC_node* root2) {
	vector<int> nodes1 = root1->traverse();
	vector<int> nodes2 = root2->traverse();
// 	double z1 = -9e99; 	for(unsigned int i=0; i<nodes1.size(); ++i) z1 = max(z1, elements[ nodes1[i] ].score);
// 	double z2 = -9e99; 	for(unsigned int j=0; j<nodes2.size(); ++j) z2 = max(z2, elements[ nodes2[j] ].score);
	double z1 = -9e99; 	for(unsigned int i=0; i<nodes1.size(); ++i) z1 += elements[ nodes1[i] ].score;
	double z2 = -9e99; 	for(unsigned int j=0; j<nodes2.size(); ++j) z2 += elements[ nodes2[j] ].score;
	if (z1 > z2) return -1;
	if (z1 < z2) return  1;
	
	int dim = elements.patterns.size() ;
	
	for(int d=0; d<dim; ++d) {
		double s1=0.0, s2=0.0;
		for(unsigned int i=0; i<nodes1.size(); ++i) s1 += elements[ nodes1[i] ].vp[d];
		s1 /= double( nodes1.size() );
		for(unsigned int i=0; i<nodes2.size(); ++i) s2 += elements[ nodes2[i] ].vp[d];
		s2 /= double( nodes2.size() );
		if (s1 > s2) return -1;
		if (s1 < s2) return  1;
		}
// 	for(unsigned int i=0; i<nodes2.size(); ++i) s2 = ;
	return 0;
	}
