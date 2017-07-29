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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

using namespace std;

#include <algorithm>
#include <vector>

#include "tepapa-clusterer.h"
#include "tepapa-utils.h"
#include "bprof.h"
#include "parse.h"
#include "bhc.h"
#include "tepapa-msg.h"
#include "heatmap.h"


const char* z_distance_metric_names[] = { "(default)", "Eucliean", "Fisher exact test", "Manhattan", 0 };
const char* z_linkage_criteria_names[] = { "(default)", "Complete-linkage", "Single-linkage", "UPGMA", "UPGMC", 0 };


TEPAPA_Program_Clusterer::TEPAPA_Program_Clusterer()
	: TEPAPA_Program("@Cluster") 
	{
	npred = 2;
	tau = 0.0;
	f_export_informarkers = false;
	
	distance_metric_index [ ght("Eucliean") ]  = BHC_DISTANCE_METRIC_EUCLIDEAN ;
	distance_metric_index [ ght("Fisher") ]    = BHC_DISTANCE_METRIC_FISHEREXACT ;
	distance_metric_index [ ght("Manhattan") ] = BHC_DISTANCE_METRIC_MANHATTAN ;
	linkage_criteria_index [ ght("Max") ]      = BHC_LINKAGE_CRITERIA_COMPLETE ;
	linkage_criteria_index [ ght("Complete") ] = BHC_LINKAGE_CRITERIA_COMPLETE ;
	linkage_criteria_index [ ght("Min") ]      = BHC_LINKAGE_CRITERIA_SINGLE ;
	linkage_criteria_index [ ght("Single") ]   = BHC_LINKAGE_CRITERIA_SINGLE ;
	linkage_criteria_index [ ght("UPGMA") ]    = BHC_LINKAGE_CRITERIA_MEAN ;
	linkage_criteria_index [ ght("Mean") ]     = BHC_LINKAGE_CRITERIA_MEAN ;
	linkage_criteria_index [ ght("UPGMC") ]    = BHC_LINKAGE_CRITERIA_CENTROID ;
	linkage_criteria_index [ ght("Centroid") ] = BHC_LINKAGE_CRITERIA_CENTROID ;
		
	// default to UPGMA with Eucliean distance
	linkage_criteria = BHC_LINKAGE_CRITERIA_MEAN;  
	distance_metric  = BHC_DISTANCE_METRIC_EUCLIDEAN; 
	
	options_optarg.push_back( 
		TEPAPA_option_optarg ("-d", "--distance-measure", "", 
			[&](const string& optarg) -> bool { distance_metric = distance_metric_index[ ght( optarg.c_str() ) ];  return true; }
			)
		); 
	
	options_optarg.push_back( 
		TEPAPA_option_optarg (
			"-l", "--linkage-method", "", 
			[&](const string& optarg) -> bool { linkage_criteria = linkage_criteria_index[ ght( optarg.c_str() ) ]; return true; }
			)
		);
	
	options_optarg.push_back( 
		TEPAPA_option_optarg ("-n", "--number-predictors", "", 'i', &npred)
		);
	
	options_optarg.push_back( 
		TEPAPA_option_optarg ("-t", "--threshold", "", 'f', &tau, 
			[&](const string& optarg) -> bool { npred = 0;  return true; }
			)
		);
	
	options_binary.push_back( 
		TEPAPA_option_binary ("-m", "--export-informakers", "Export as informarkers instead of labelling of groups only", 'b', &f_export_informarkers)
		); 
	
	options_optarg.push_back( 
		TEPAPA_option_optarg ("-h", "--heatmap", "", 'b', &heatmap_filename)
		); 
	
	}



int TEPAPA_Program_Clusterer::run(TEPAPA_dataset&  ds_input, TEPAPA_Results&  rr_output) {
	
	int retval = TEPAPA_RETVAL_SUCCESS;

	if ( (linkage_criteria <=0) || ((unsigned)linkage_criteria >= sizeof(z_linkage_criteria_names) / sizeof(z_linkage_criteria_names[0]) ) ) {
		msgf(VL_FATAL, "Invalid linkage criteria selected.");
		}

	if ( (distance_metric <=0) || ((unsigned)distance_metric >= sizeof(z_distance_metric_names) / sizeof(z_distance_metric_names[0]) ) ) {
		msgf(VL_FATAL, "Invalid distance metric selected.");
		}

	msgf(VL_NOTICE, "Using %s with %s\n", z_linkage_criteria_names[linkage_criteria], z_distance_metric_names[distance_metric] );
	
	// copy results from the input dataset
	TEPAPA_Results  rr = ds_input.rr; 
	
	vector< vector<int> >  clusters ;
	
	BHC_Tree bhtree;
	
	if ( rr.size() <= 1 ) {
		msgf(VL_WARNING, "No results to cluster!");
		return TEPAPA_RETVAL_SUCCESS;
		}
	else if ( (npred != 0) && (rr.size() <= (unsigned int) npred) ) {
		for(unsigned int i=0; i<rr.size(); ++i) {
			vector<int> v;
			v.push_back(i);
			clusters.push_back(v);
			}
		}
	else {
// 		BHC_Tree bhtree(rr, distance_metric, linkage_criteria);
	
		bhtree.gentree(rr, distance_metric, linkage_criteria);
	
		if (!bhtree)  msgf(VL_FATAL, "Clustering failed.\n");
	
		msgf(VL_INFO, "Gentree done.\n");
		
// 		bhtree.dump(rr);
		
		vector<double> thres = bhtree.get_thresholds();
		
// 		for(unsigned int i=0; i<thres.size(); ++i) printf("%d\t%f\n", i, thres[i]);
		
		if (npred != 0) tau = thres[npred-1] ;
		
		clusters = bhtree.do_cluster( tau );
		
		msgf(VL_INFO, "Groups = %d\n", npred);
		msgf(VL_INFO, "Grouping similarity threshold = %f\n", tau );
		}
	


	TEPAPA_Results  informarkers;
	
	for(unsigned int g=0; g<clusters.size(); ++g) {
		printf("GROUP %d:\n", g + 1);
		
		int best_z = -1;
		double best_pval = 9e99;
		
		
		for(unsigned int k=0; k<clusters[g].size(); ++k) {
			int z = clusters[g][k];
			TEPAPA_Result& rrz = rr[z];
			
			rrz.group=(g+1);
			
			if (rrz.pval < best_pval) {
				best_pval = rrz.pval ;
				best_z = z;
				}
			}
		
		informarkers.push_back(rr[best_z]);
		rr[best_z].sel = true;
		
		for(unsigned int k=0; k<clusters[g].size(); ++k) {
			int z = clusters[g][k];
			TEPAPA_Result& rrz = rr[z];
			
			if (z == best_z) printf ("  ***");
			printf("\t[%d]", z);
			printf("\t%8.6f", rrz.est);
			printf("\t%9.4g", rrz.pval);
			printf("\t%s", rrz.bprof.digest().c_str() );
			printf("\t%s", rrz.patt->to_string().c_str() );
			printf("\n");
// 			if (z == best_z) printf ("\e[0m");
			}
		printf ("\n");
		}
	
	if ( f_export_informarkers ) {
		sort(informarkers.begin(), informarkers.end(), sort_by_pval);
			
		for(unsigned int i=0; i<informarkers.size(); ++i ){
			msgf(VL_INFO, "INFORMARKER\t%d\t%8.6f\t%9.4g\t%s\t%s\n", 
				i + 1, informarkers[i].est, 
				informarkers[i].pval, informarkers[i].bprof.digest().c_str(), 
				informarkers[i].patt->to_string().c_str()
				);
			}
		
		rr_output = informarkers;
		retval = TEPAPA_RETVAL_SUCCESS ;
		}
	else {
		rr_output = rr;
		retval = TEPAPA_RETVAL_SUCCESS ; // | TEPAPA_RETVAL_TERMINUS;
		}
	

	if ( heatmap_filename.size() ) {
		string heatmap_out; 
		
		TEPAPA_dataset  ds = ds_input;
		ds.rr = rr;
		
		heatmap hm(ds);
		
		if ( hm.dump(heatmap_out, bhtree) ) {
			FILE* fout ;
			if ( ! ( fout = fopen(heatmap_filename.c_str(), "wt") ) ){
				msgf(VL_FATAL, "Unable to write heatmap %s", heatmap_filename.c_str());
				return TEPAPA_RETVAL_FAILURE;
				}
			fprintf(fout, "%s", heatmap_out.c_str() );
			fclose(fout);
			}
		else {
			msgf(VL_WARNING, "Failed to generate heatmap.\n", heatmap_filename.c_str());
			}
		}
	
	return retval ;
	}
