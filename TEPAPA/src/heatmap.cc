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

#include "heatmap.h"
#include "stats.h"
#include "utils.h"

double heatmap::dump_dendrogram_r(string& out, const BHC_node* root, 
	const TEPAPA_Results& patterns, vector<int>& reorder,
	double& diff_max, double nearest_diff, int delta_start
	) const {
	unsigned int row_cnt = reorder.size(); 
	
	if (root->id >= 0) {
		out += strfmt("segments(%f, %f, %f, %f);\n", 0.0, double(row_cnt+delta_start), nearest_diff, double(row_cnt+delta_start));
		double retval = double(row_cnt);
		reorder.resize(row_cnt + 1);
		reorder[row_cnt] = root->id;
		++row_cnt;
		return retval ;
		}
	
	double row_l=0, row_r=0;
	
	if ( diff_max < root->diff ) diff_max = root->diff ;
	
	if (root->left)  row_l = dump_dendrogram_r(out, root->left,  patterns, reorder, diff_max, root->diff, delta_start);
	if (root->right) row_r = dump_dendrogram_r(out, root->right, patterns, reorder, diff_max, root->diff, delta_start);
	
	double row_m = ( (row_l + row_r) / 2);
	
	out += strfmt("segments(%f, %f, %f, %f);\n", root->diff, row_l, root->diff,   row_r );
	out += strfmt("segments(%f, %f, %f, %f);\n", root->diff, row_m, nearest_diff, row_m );
	
	return row_m;
	}


	
double heatmap::dump_dendrogram_r(string& out, const BHC_node* root, 
	const feature_arrays& patterns, vector<int>& reorder, 
	double& diff_max, double nearest_diff, int delta_start
	) const {
	unsigned int col_cnt = reorder.size(); 
	
	assert(root); 
	
	if (root->id >= 0) {
		out += strfmt("segments(%f, %f, %f, %f);\n", double(col_cnt+delta_start), 0.0, double(col_cnt+delta_start), nearest_diff);
		double retval = double(col_cnt+delta_start);
		reorder.resize(col_cnt + 1);
		reorder[col_cnt] = root->id;
		++col_cnt;
		return retval ;
		}
	
	double col_l=0, col_r=0;
	
	if ( diff_max < root->diff ) diff_max = root->diff ;
	
	if (root->left)  col_l = dump_dendrogram_r(out, root->left,  patterns, reorder, diff_max, root->diff, delta_start);
	if (root->right) col_r = dump_dendrogram_r(out, root->right, patterns, reorder, diff_max, root->diff, delta_start);
	
	double col_m = ( (col_l + col_r) / 2);
	
	out += strfmt("segments(%f, %f, %f, %f);\n", col_l, root->diff, col_r, root->diff );
	out += strfmt("segments(%f, %f, %f, %f);\n", col_m, root->diff, col_m, nearest_diff );
	
	return col_m;
	}
	
	
	
const double pixel_width = 0.5;
	
bool heatmap::dump(string& out, BHC_Tree& bht_features) {
	
	if ( ! ds.rr.size() ) {
		msgf(VL_WARNING, "Heatmap: no features to dump!\n");
		return false;
		}
		
	if ( ! ds.sl.size() ) {
		msgf(VL_WARNING, "Heatmap: no samples to dump!\n");
		return false;
		}
	
	double diff_max_x = 0.0, diff_max_y = 0.0;
	vector<int> reorder_x = seq<int>( 0, ds.sl.size()-1, 1) ;
	vector<int> reorder_y;
	
	string dendrogram_out_x, dendrogram_out_y, sigplot_out, heatmap_out, sample_legend_out, sample_labels_out, labels_out;
		
	string header = "\n\
	pdf(width=9.7, height=6.2);\n\
	par(oma=c(1,1,1,1), mar=c(0,0,0,0));\n\
	layout(t(matrix(c(1,2,3,4,5,6,7,8,9,10,11,12), 4)), width=c(1,1,8,3), height=c(3,20,3));\n\
	";

	vector<double> score_levels = get_levels( ds.sl.get_scores() );

	unsigned int b=0, e=0;
	
	feature_arrays  vfa(ds);
	
	while( b<ds.sl.size() ) {
		for(e=b; e<=ds.sl.size(); ++e) {
			if (e >= ds.sl.size()) break;
			if ( ds.sl[b].score != ds.sl[e].score ) break;
			}
			
		if ( (e - b) == 1 ) {
			b = e;
			continue;
			}
		
		BHC_Tree bhtree_samples;

		bhtree_samples.gentree(vfa, BHC_DISTANCE_METRIC_EUCLIDEAN, BHC_LINKAGE_CRITERIA_COMPLETE, b, e);
	
		if (!bhtree_samples)  msgf(VL_FATAL, "UPGMA failed.\n");
		
		msgf(VL_INFO, "Gentree done.\n");
		
		bhtree_samples.dump(vfa);
		
		string dendrogram_out_x_1;
		vector<int> reorder_x_1 ;
		double diff_max_x_1 = 0.0;

// 		printf( "b=%d\te=%d\treorder_x_1.size()=%d\n", b, e, reorder_x_1.size());
		
		dump_dendrogram(dendrogram_out_x_1, bhtree_samples, vfa, diff_max_x_1, reorder_x_1, b);
		diff_max_x = max(diff_max_x, diff_max_x_1);
		dendrogram_out_x += dendrogram_out_x_1;
		
// 		print( reorder_x_1 );

		for(unsigned int i=0; i<reorder_x_1.size(); ++i) reorder_x[b+i] = reorder_x_1[i];
	
		b = e;
		}
	
	print( reorder_x );

// // 	
// 
// 	printf("\n");
	
	
// 	for(int i=0; i<score_levels.size(); ++i) printf("LEVEL %d\t%f\n", i, double(score_levels[i]));
	
	string dummy_header = "plot.new();\n";
	
	string sample_legend_header = strfmt( 
		"plot.new();\nplot.window(xlim=c(0,%g), ylim=c(0,%g));\n", 
		double(ds.sl.size()+1), diff_max_x
		);

	for(unsigned int i=0; i<ds.sl.size(); ++i) {
		double frac_rank = ranked_level(score_levels, ds.sl[i].score) ;
// 		const string colstr = ( ds.sl[i].score == 1.0 ? "\"darkgreen\"" : "\"darkred\"" );
		double brightness = 0.15;
		double r = min( 1.0, ( (1.0-frac_rank) + brightness ) );
		double g = min( 1.0, ( 0.60 * frac_rank + brightness) ) ;
		double b = min( 1.0, ( brightness) ) ;
		const string colstr = strfmt( "rgb(%g,%g,%g)", r, g, b);
		
		int ri = reorder_x[i]; 
		double rx = ri; 
		
		sample_legend_out += strfmt ("rect(%g, %g, %g, %g, col=%s, border=NA);\n", 
			rx-pixel_width, double(0), rx+pixel_width, double(-1), colstr.c_str() );
		
		heatmap_out += strfmt ("rect(%g, %g, %g, %g, col=%s, border=NA);\n", 
			rx-pixel_width, double(0)-pixel_width, rx+pixel_width, double(ds.rr.size())-pixel_width, colstr.c_str());
		}
	
	
	dump_dendrogram(dendrogram_out_y, bht_features, ds.rr, diff_max_y, reorder_y);
	
// 		heatmap_out += strfmt ("rect(%f, %f, %f, %f, col=\"%s\", border=NA);\n", 
// 			double(0)-0.5, double(0)-0.5, double(ds.sl.size())-0.5, double(reorder_y.size())-0.5, col);

	double max_log10_pval = 0.00;

	vector<double> log10pvals;
	for(unsigned int j=0; j<reorder_y.size(); ++j) {
		double pval = ds.rr[ reorder_y[j] ].pval;
		double log10_pval = -log( (pval < 1e-100) ? 1e-100 : pval) / log(10);
		log10pvals.push_back(log10_pval);
		if (max_log10_pval < log10_pval) max_log10_pval = log10_pval  ;
		}

// 	// grid lines x
// 	if (ds.sl.size() < 100 ) { // to avoid cluttering
// 		for(unsigned int i=0; i<=ds.sl.size(); ++i) {
// 			heatmap_out += strfmt("segments(%g, %g, %g, %g, col='white');\n", 
// 				double(i)-0.5, double(0)-0.5, double(i)-0.5, double(ds.rr.size())-0.5
// 				);
// 			}
// 		}
// 	// grid lines y
// 	if (ds.rr.size() < 100 ) { // to avoid cluttering
// 		for(unsigned int j=0; j<=ds.rr.size(); ++j) {
// 			heatmap_out += strfmt("segments(%g, %g, %g, %g, col='white');\n", 
// 				double(0)-0.5, double(j)-0.5, double(ds.sl.size())-0.5, double(j)-0.5
// 				);
// 			}
// 		}
		
	for(unsigned int j=0; j<reorder_y.size(); ++j) {
		double log10_pval = log10pvals[j];
		const TEPAPA_Result& rrrj = ds.rr[ reorder_y[j] ];
		int group = rrrj.group;
		
		double mid = ( rrrj.method == ght("AUC") || rrrj.method == ght("AUROC2") ) ? 0.5 : 0.0 ;
		
		bool flag = (rrrj.est > mid);
		
		if ( rrrj.sel ) {
// 			sigplot_out += strfmt ("rect(%g, %g, %g, %g, col=\"lightyellow\", border=NA);\n", double(0), double(j)-0.5, double(0)+max_log10_pval, double(j)+0.5);
			sigplot_out += strfmt ("points(%g, %g, col=\"%s\", pch=16, cex=1.5);\n", double(log10_pval), double(j), flag ? "darkgreen" : "red" );
			}
		else {
			sigplot_out += strfmt ("points(%g, %g, col=\"%s\", pch=1);\n", double(log10_pval), double(j), "black" );
			}
		
		sigplot_out += strfmt ("rect(%g, %g, %g, %g, col=%d, border=NA);\n", double(0), double(j)-0.5, double(0)-(max_log10_pval/10.0), double(j)+0.5, group + 1);
		}
	
	
	for(unsigned int i=0; i<ds.sl.size(); ++i) {
		int ri = reorder_x[i]; 
		double rx = ri; 

		for(unsigned int j=0; j<reorder_y.size(); ++j) {
			
			int rj = reorder_y[j]; 
			double ry = rj; 
			
			const TEPAPA_Result& rrrj = ds.rr[ rj ];
			bool flag = rrrj.bprof[ ri ];
			bool informarker = rrrj.sel;
			
			string colstring ; 
			
			if (!flag) continue;
			
			if ( ! rrrj.is_numeric() ) {
// 				printf("! %d\n", rrrj.val.size() );
				colstring = (informarker ? "\"yellow\"" : "\"orange\"");
				heatmap_out += strfmt ("rect(%g, %g, %g, %g, col=%s, border=NA);\n", 
					double(i)-pixel_width, double(j)-pixel_width, 
					double(i)+pixel_width, double(j)+pixel_width, colstring.c_str()
// 					double(rx)-pixel_width, double(ry)-pixel_width, 
// 					double(rx)+pixel_width, double(ry)+pixel_width, colstring.c_str()
					);
				}
			else {
				double v = rrrj.val[i];
				if (v > 1.0) v = 1;
				if (v < 0.0) v = 0;
				double intensity = ( (v * 0.95) + 0.05 );
				if (intensity > 1) intensity = 1;
				if (intensity < 0.5) intensity = 0.5;
				
				colstring = strfmt( "rgb(%g,%g,%g)", 
					intensity, (informarker ? intensity : intensity * 0.60 ), 0.25 * (1.0 - v) );
				heatmap_out += strfmt ("rect(%g, %g, %g, %g, col=%s, border=NA);\n", 
// 					double(rx)-pixel_width, double(ry)-pixel_width, 
// 					double(rx)+pixel_width, double(ry)+pixel_width, 
					double(i)-pixel_width, double(j)-pixel_width, 
					double(i)+pixel_width, double(j)+pixel_width, 
					colstring.c_str()
					);
				}
			}
		}
		

	for(unsigned int j=0; j<ds.rr.size(); ++j) {
		unsigned int rj = reorder_y[j]; 
		double ry = rj; 
		
		const TEPAPA_Result& rrrj = ds.rr[ rj ];
		const pattern* p = rrrj.patt;
		bool informarker = rrrj.sel;
		string label = ght[p->get_type_id()] + string(":") + p->to_string();
		size_t z ;
		if ( (z = label.find('"')) != string::npos ) label[z] = ' ';
		
		int group = rrrj.group;
		
		labels_out += strfmt ("rect(%g, %g, %g, %g, col=%d, border=NA);\n", double(0), j-pixel_width, double(0)-0.1, j+pixel_width, group + 1);

		double mid = ( rrrj.method == ght("AUC") || rrrj.method == ght("AUROC2") ) ? 0.5 : 0.0 ;
		
		bool flag = (rrrj.est > mid);
		
		if (informarker) {
			labels_out += strfmt( "text(x=%g, y=%g, \"%s\", adj=0, col='%s');\n", double(0), double(j), label.c_str(), flag ? "darkgreen" : "red" );
			}
		else {
			if (reorder_y.size() < 40 ) { // to avoid cluttering
				labels_out += strfmt( "text(x=%g, y=%g, \"%s\", adj=0);\n", double(0), double(j), label.c_str() );
				}
			}
		}
	
	// X label names
	for(unsigned int i=0; i<ds.sl.size(); ++i) {
		unsigned int ri = reorder_x[i]; 
		double rx = ri; 
		assert(ri>=0);
		assert(ri<ds.sl.size());
		string def_str = ght[ ds.sl[ri].definitions[0] ];
		size_t z = def_str.rfind('/');
		string label ;
		label = ( ( z == string::npos ) ? def_str : def_str.substr(z+1) );
		
		sample_labels_out += strfmt ("text(x=%g, y=1, \"%s\", adj=1, srt=90);\n", double(i), label.c_str() );
		}
	
	string dendrogram_header = strfmt( 
		"plot.new();\nplot.window(xlim=c(%g,0), ylim=c(%g,0));\n", 
		diff_max_y,
		double(ds.rr.size()+1)
		);

	string sigplot_header = strfmt( 
		"plot.new();\nplot.window(xlim=c(0,%g), ylim=c(%g,0));\naxis(1);\nmtext(\"-log(p)\", side=1, line=2, cex=0.75);\n", 
		max_log10_pval,
		double(ds.rr.size()+1)
		);
	
	string heatmap_header = strfmt( 
		"plot.new();\nplot.window(xlim=c(0,%g), ylim=c(%g,0));\n",
		double(ds.sl.size()+1),
		double(ds.rr.size()+1)
		);
		
	string labels_header = strfmt( 
		"plot.new();\nplot.window(xlim=c(0,1), ylim=c(%g,0));\n",
		double(ds.rr.size()+1)
		);
		
	string sample_labels_header = strfmt( 
		"plot.new();\nplot.window(xlim=c(0,%g), ylim=c(0,1));\n",
		double(ds.sl.size()+1)
		);
		
	out = header              +
		dummy_header          + 
		dummy_header          + 
		sample_legend_header  + sample_legend_out + dendrogram_out_x +
		dummy_header          + 
		dendrogram_header     + dendrogram_out_y  + 
		sigplot_header        + sigplot_out       +
		heatmap_header        + heatmap_out       +
		labels_header         + labels_out        +
		dummy_header          + 
		dummy_header          + 
		sample_labels_header  + sample_labels_out +
		dummy_header ;
	
// 	printf ("%s", out.c_str() );
	
	return true;
	}
