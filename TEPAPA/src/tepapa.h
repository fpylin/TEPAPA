/* tepapa.h
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

#ifndef __tepapa_h
#define __tepapa_h 1

#define VERSION_STRING      "0.99" 

#include "hashtable.h"
#include "stopwatch.h"
#include <set>

extern std::string           prog_path;
extern std::string           sub_program;
extern stopwatch             master_clock;
extern std::set<hash_value>  hv_separators;
extern hash_value            hv_wildcard;
extern hash_value            hv_class_NUMBER ;

#define TEPAPA_RETVAL_FAILURE         0x0000
#define TEPAPA_RETVAL_SUCCESS         0x0001
#define TEPAPA_RETVAL_SUCCESS_MASK    0x0001
#define TEPAPA_RETVAL_JUMP            0x0002
#define TEPAPA_RETVAL_JUMP_MASK       0x0002
#define TEPAPA_RETVAL_TERMINUS        0x0010

#define DEFAULT_DATASET_NAME             "default"

#define GVAR_CURRENT_STEP                "CurrentStep"
#define GVAR_GOTO_STEP                   "GotoStepNumber"

#define GVAR_ACTIVE_DATASET              "ActiveDataset"
#define GVAR_ACTIVE_DATASET_TRAINING     "ActiveDatasetTraining"
#define GVAR_ACTIVE_DATASET_TESTING      "ActiveDatasetTesting"

#define GVAR_ACTIVE_DATASET_PREDICTIONS_STEM          "ActiveDatasetPredictionStem"

#define GVAR_ACTIVE_DATASET_PREDICTIONS_CSV           "ActiveDatasetPredictionCSV"
#define GVAR_ACTIVE_DATASET_PREDICTIONS_TRAINING_CSV  "ActiveDatasetPredictionTrainingCSV"
#define GVAR_ACTIVE_DATASET_PREDICTIONS_TESTING_CSV   "ActiveDatasetPredictionTestingCSV"

#define GVAR_ACTIVE_DATASET_PREDICTIONS_ARFF          "ActiveDatasetPredictionARFF"
#define GVAR_ACTIVE_DATASET_PREDICTIONS_TRAINING_ARFF "ActiveDatasetPredictionTrainingARFF"
#define GVAR_ACTIVE_DATASET_PREDICTIONS_TESTING_ARFF  "ActiveDatasetPredictionTestingARFF"


#endif 
