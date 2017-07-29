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
#include "roc.h"
#include <assert.h>

two_class_stats_with_threshold    roc_curve::dummy;

#include <map>
#include <algorithm>
#include <gsl/gsl_cdf.h>
#include <math.h>

using namespace std;


void roc_curve::compute_roc_curve_proper(vector<pc_pair>& data) {
	clear();
	
 	sort(data.begin(), data.end()); // quick sort the data
	
	double threshold = +INFINITY;
	int cp = 0; // current condition positive
	int n  = 0; // number of items tested
	int np = 0; // total number of predictors positive

	int data_size = (int)(data.size());
	
	for(int i=0; i<data_size; ++i) {
		if (data[i].c) np++;
		}
	
	for(int i=data_size-1; i>=0; --i, ++n) {
		double tau = data[i].p;
		if (tau != threshold) // threshold changed
			{
			two_class_stats_with_threshold  s;
			s.threshold = threshold;
			s.tp = cp;       s.fp = n - cp;
			s.fn = np - cp;  s.tn = i + 1 - (np - cp);
			insert(s);
			threshold = tau;
			}
		
		if (data[i].c) ++cp;
		}

	two_class_stats_with_threshold  s;
	s.threshold = -INFINITY;
	s.tp = np;  s.fp = n - np;
	s.fn = 0;   s.tn = 0;
	insert(s);

	threshold_stats.clear();
	}


/* New algorithm */
void roc_curve::compute( const vector<double>& v_cond, const vector<double>& v_pred)
	{
	/* get thresholds */
	vector<pc_pair> data;
	
	assert(v_cond.size() == v_pred.size());
	
	for(unsigned int i=0; i<v_cond.size(); ++i)
		{
		if ( isnanf(v_cond[i]) || isnanf(v_pred[i]) ) continue;
		
		pc_pair d(v_pred[i], v_cond[i]);
		
		data.push_back(d);
		}
	
	compute_roc_curve_proper(data);
	}


void roc_curve::compute( const vector<bool>& v_cond, const vector<double>& v_pred)
	{
	/* get thresholds */
	vector<pc_pair> data;
	
	assert(v_cond.size() == v_pred.size());
	
	for(unsigned int i=0; i<v_cond.size(); ++i)
		{
		pc_pair d(v_pred[i], v_cond[i]);
		
		data.push_back(d);
		}
	
	compute_roc_curve_proper(data);
	}



/**
 * @name  AUC
 */


double roc_curve::AUC (double* pvalue) const
	{
	double last_tpr = 0;
	double last_fpr = 0;
	double area     = 0.0;
	
/*	vector<two_class_stats_with_threshold> vec_roc;
	for(set<two_class_stats_with_threshold>::const_iterator r=roc.begin(); r!=roc.end(); ++r) vec_roc.push_back(*r);
	sort(vec_roc.begin(), vec_roc.end());
*/
	
	for(const_iterator r=begin(); r!=end(); ++r) {
		double tpr = r->tpr();
		double fpr = r->fpr();
		
// 		printf("%f\t%f\t%f\t%c\n", tpr, fpr, last_fpr, (fpr < last_fpr) ? '+':'-');
// 		if (fpr < last_fpr) abort();
		
		area += (last_tpr + tpr)  *  (fpr - last_fpr)/ 2.00;
		
		last_fpr = fpr;
		last_tpr = tpr;
		}
	
	area += (1.00 + last_tpr) * (1.00-last_fpr) / 2.00;
	
	if (pvalue) {
		double n1  = begin()->tp + begin()->fn;
		double n2  = begin()->fp + begin()->tn;
		double U_a = area * n1 * n2; 
		double U_n = (1 - area) * n1 * n2; 
		double se_U = sqrt ( n1 * n2 * ( n1 + n2 + 1) / 12) ;
		double U_mu = (U_a + U_n) / 2;
		double z = (U_a - U_mu) / se_U ;
    	double p = 1 - fabs(0.5 - gsl_cdf_ugaussian_P(z)) * 2;
    	if (p == 0.0) p = gsl_cdf_ugaussian_Q( fabs(z) ) * 2;
// 		printf("\n%.0f\t%.0f\t%f\t%f\n", n1, n2, z, p);
		*pvalue = p;
		}
	
	return area;
	}


double roc_curve::AUC(double& ci95l, double& ci95h) const {
	double area = AUC();
	double se = SE_AUC( area, begin()->dp(), begin()->tot());
	
	ci95l = area - 1.96 * se;
	ci95h = area + 1.96 * se;
	
	return area;
	}


double roc_curve::SE_AUC(double auc, double na, double nn) {
	double auc2 = auc * auc;
	double q1 = auc / (2.0 - auc);
	double q2 = 2.0 * auc2 / (1.0 + auc);
	double SE2 = (auc * (1-auc) +
			(na - 1) * (q1 - auc2) +
			(nn - 1) * (q2 - auc2) ) /
			(na * nn);
	return sqrt(SE2);
	}


double roc_curve::optimal_cutoff(double& max_Hm, double& max_Hm_sens, double& max_Hm_spec, double factor) const {
	max_Hm = 0.5000;
	max_Hm_sens = -1.00;
	max_Hm_spec = -1.00;
	double optimal_cutoff = -1.00;
	
	for (const_iterator i=begin(); i!=end(); ++i) {
		if (i->h_measure() * factor > max_Hm * factor) {
			optimal_cutoff = i->threshold;
			max_Hm = i->h_measure();
			max_Hm_sens = i->sens();
			max_Hm_spec = i->spec();
			}
		}
	return optimal_cutoff ;
	}


double roc_curve::optimal_cutoff(bool reversed) const {
	double v = 0.5000;
	double optimal_cutoff = -1.00;
	
	for (const_iterator i=begin(); i!=end(); ++i) {
		if ( (reversed && ( i->eauc()  < v ) ) || (!reversed && ( i->eauc() > v ) ) ) {
			optimal_cutoff = i->threshold;
			v = i->eauc() ;
			}
		}
	return optimal_cutoff ;
	}



/* return the cut-off point where spec > x*/
double roc_curve::at_spec(double x, bool reversed, double* corr_eauc) const {
	if (reversed) {
		for (const_iterator i=begin(); i!=end(); ++i) {
// 			fprintf(stderr, "%.5f\t%.5f\n", i->threshold, i->spec());
			if ( i->spec() <= (1-x)) {
				if (corr_eauc) *corr_eauc = (1-i->eauc()); // ( reversed ? i->sens() : 1-i->sens() );
				return i->threshold;
				}
			}
		}
	else
		{
		for (const_reverse_iterator i=rbegin(); i!=rend(); ++i) {
			if ( i->spec() >= x) {
				if (corr_eauc) *corr_eauc = i->eauc(); // ( reversed ? i->sens() : 1-i->sens() );
				return i->threshold;
				}
			}
		}
	return -1.00;
	}

// r->print("HFefghabcdjJl");


void roc_curve::print_thresholds(const string& fmt, FILE* file) const {
	for(const_iterator r=begin(); r!=end(); ++r) {
		fprintf(file, "  thres %8.20f ",r->threshold);
		r->print( fmt, file );
		fprintf(file, "\n");
		}
	}



/**
 */


void print_roc_stats (
	const vector<double>& v_cond,
	const vector<double>& v_pred
	)
	{
	double auc, auc_ci95l, auc_ci95h;
	
	roc_curve  roc ( v_cond, v_pred );
	
	auc = roc.AUC(auc_ci95l, auc_ci95h);
		
	double max_Hm, max_Hm_sens, max_Hm_spec;
	
	double optimal_cutoff = roc.optimal_cutoff(max_Hm, max_Hm_sens, max_Hm_spec);
	
	printf("  AUC        : %.4f ( %.4f -- %.4f ) \n", auc, auc_ci95l, auc_ci95h);
	printf("  Cut-off    : %.4f \n", optimal_cutoff);
	printf("  Max Hm     : %.4f \n", max_Hm);
	printf("  opt. sens  : %.4f \n", max_Hm_sens);
	printf("  opt. spec  : %.4f \n", max_Hm_spec);
	}


const two_class_stats_with_threshold& roc_curve::find_threshold(double tau) {
	if ( !size() ) return dummy;
	
	if ( !threshold_stats.size() ) {
		set<two_class_stats_with_threshold>::const_iterator t, e=end();
		
		for(t=begin(); t!=e; ++t) threshold_stats.push_back(*t);
		}
	
	
	register int n=threshold_stats.size();
	register int i=0, j=n-1;
	register int m = (i+j)/2;
	
	do	{
		register double th_m = threshold_stats[m].threshold ;
		
		if (th_m == tau) break;
		else if (th_m > tau) i = m+1;
		else if (th_m < tau) j = m-1;
		else { i = m; break; }
		
#if DEBUG
		fprintf(stderr, "\n{i=%2d (%f)\tj=%2d (%f)\tm=%2d (%f)\ttau=%f}", 
			i, threshold_stats[i].threshold,
			j, threshold_stats[j].threshold,
			m, threshold_stats[m].threshold,
			tau);
#endif
/*		if (m>=n) { m = n-1; break; }
		else if (m<0) { m=0; break; }*/
		m = (i+j)/2;
		}
	while(j-i > 0);
	
#if DEBUG
	fprintf(stderr, "\n<%d\t%d\t%f\t%f>\n", threshold_stats.size(), m, tau, threshold_stats[m].threshold);
	threshold_stats[m].print("abcdefgh", stderr);
	fprintf(stderr, "\n\n");
#endif
	
	return threshold_stats[m];
	}

	
	

void roc_curve_kernel::calculate_kernel_densities( const vector<double>& v_cond, const vector<double>& v_pred, double bw_frac) {
	vector<double> v_pos, v_neg; 
	
	vector<pc_pair> data;
	
	assert( v_cond.size() == v_pred.size() );
	
	for(unsigned int i=0; i<v_cond.size(); ++i)
		{
		if ( isnanf(v_cond[i]) || isnanf(v_pred[i]) ) continue;
// 		if ( isinf((double)v_cond[i])  || isinf((double)v_pred[i]) ) continue;
		
		if ( v_cond[i] != 0.0 ) 
			v_pos.push_back(v_pred[i]);
		else 
			v_neg.push_back(v_pred[i]);
		}

// 	printf("!! %d\n", v_pos.size());
// 	for(int i=0; i<v_pos.size(); ++i) printf("%f ", v_pos[i]);
// 	printf("\n");
	
// 	printf("!! %d\n", v_neg.size());
// 	for(int i=0; i<v_neg.size(); ++i) printf("%f ", v_neg[i]);
// 	printf("\n");

	posex = new gaussian_kde(v_pos);
	posex->set_eval_points( posex->estimate_bw() / bw_frac); // 
	negex = new gaussian_kde(v_neg);
	negex->set_eval_points( negex ->estimate_bw() / bw_frac);
	prevalence = double(v_pos.size()) / double(v_pos.size() + v_neg.size()) ;
	}


void roc_curve_kernel::print_thresholds_2(const string& fmt, FILE* file) 
	{
	double auck = 0.0;
	double se0 = -1;
	double sp0 = -1;
	for(const_iterator r=begin(); r!=end(); ++r) {
		fprintf(file, "  thres %8.10f ",r->threshold);
		double se = sens(r->threshold) ;
		double sp = spec(r->threshold) ;
		fprintf(file, "kse %5.3f ", se);
		fprintf(file, "ksp %5.3f ", sp);
		double lrp = se / (1 - sp);
		fprintf(file, "klrp %5.5g ", lrp);
		double ppv = se * prevalence / (se * prevalence + (1 - sp) * (1 - prevalence) );
		fprintf(file, "kpp %5.3f ", ppv);
	
		r->print( fmt, file );
		fprintf(file, "\n");
		if (se0 != -1) auck += 0.5 * (se + se0) * fabs(sp - sp0);
		se0 = se;
		sp0 = sp;
		}
	fprintf(file, "AUC(kernel) = %.6f\n", auck);
	}
