#!/usr/bin/perl -e@A=@ARGV;$A[0]=~s%(.*)/(.+)$%$1/../bin/tepapa%;exec($A[0],$ARGV[0],@ARGV[1..$#A])
#
# tepapa-pl-simple.tpp: a simple, example TEPAPA pipeline
#
#  Copyright (C) 2015-2016, Frank Lin - All Rights Reserved
#  Written by Frank P.Y. Lin 
#  
#  This file is part of TEPAPA, a Text mining-based Exploratory Pattern
#  Analyser for Prognostic and Associative factor discovery
#  
#  TEPAPA is free software: you can redistribute it and/or modify
#  it under the terms of the GNU General Public License as published by
#  the Free Software Foundation, either version 3 of the License, or
#  (at your option) any later version.
#  
#  TEPAPA is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#  
#  You should have received a copy of the GNU General Public License
#  along with TEPAPA.  If not, see <http://www.gnu.org/licenses/>
#
# 
# SYNOPSIS
# 
#    ./tepapa-pl-simple.tpp <case_file> <filtering_pvalue> <feature_reduction_grouping> <sort_flags>
#  
#  
# EXAMPLES USAGE
# 
# 1. To identify prognostic or associative factors from EMR files listed in the ``case.txt'' 
#    (format see below) limit to all patterns p<0.05, reduce pattern by occurrence profiles (b), and 
#    sort output by direction ('d', positive/negative), p-value ('p'), statistical estimates ('e'), 
#    and number of cases containing the given pattern ('n'):
# 
#    ./tepapa-pl-simple.tpp case.txt 0.05 b dpen
#    
# 2. The above but less verbose (-q) or more verbose (-v [level]) output.
# 
#    ./tepapa-pl-simple.tpp case.txt 0.05 b dpen -q
#    ./tepapa-pl-simple.tpp case.txt 0.05 b dpen -v 7
# 
#
# FORMAT OF ``case.txt''
# 
# The format of case.txt is: 
#    score <SPACE> raw_text_file <NEWLINE>
#    score <SPACE> raw_text_file <NEWLINE>
#    score <SPACE> raw_text_file <NEWLINE>
#    ...
#
# where the "score" is the value of outcome variable associate with each raw_text_file listed above. 
# 
# For example, for binary cases, use 0 to indicate FALSE and 1 to indicate TRUE.
#  --- BEGIN OF case.txt ---
#  0    cases/case0001.txt
#  0    cases/case0002.txt
#  1    cases/case0003.txt
#  1    cases/case0004.txt
#  1    cases/case0005.txt
#  1    cases/case0006.txt
#  ...
#  --- END OF case.txt ---
#    
# For numeric outcome variables (e.g. survival days), set the score to the value 
#  --- BEGIN OF case.txt ---
#  64   patients/case0001.txt
#  132  patients/case0002.txt
#  56   patients/case0003.txt
#  245  patients/case0004.txt
#  11   patients/case0005.txt
#  15   patients/case0006.txt
#  ...
#  --- END OF case.txt ---
#    
# ``cases000x.txt'' contain the raw EMR in plain text format.
#
#
# SAMPLE OUTPUT
# 
#   --------------------------------------------------------------------------------------------------------------------------------------------
#   Group   Dir.    Method  Estimate        P-value         N       Type    Occurrence profile      Pattern
#   --------------------------------------------------------------------------------------------------------------------------------------------
#   G0010   POS     LOR.FET 2.2609404       5.25597e-05     31      NGRAM   5BB326B8BB5C0000E008    base  of  tongue
#   G0014   POS     LOR.FET 3.1388331       5.68851e-05     21      REGEX   CAA00E8AF50C00000800    of  the?  (right|left)?  (tonsil|tongue)  -
#   ...
#   G0001   NEG     LOR.FET -3.5156652      0.000374902     8       REGEX   00000000000018C09120    (of  the)  left?  (oral  tongue)
#   --------------------------------------------------------------------------------------------------------------------------------------------
#   
#   

@Set -e Input            $ARGV1
@Set -e Siglevel         $ARGV2
@Set -e FeatureGroupFmt  $ARGV3
@Set -e SortOrder        $ARGV4

#### Preprocessing  ###################################################################
# Pre-processing creates an intermediate file (in this case ``/tmp/tepapa-pl-preprocessed.txt'') 
# with the following format:
# 
#   Token1 <TAB> Tag1 <TAB> Tag2 ... <NEWLINE>
#   Token2 <TAB> Tag1 ... <NEWLINE>
#   Token3 <TAB> Tag1 <NEWLINE>
#   ...
# 

# Preprocessing using case-sensitive tokens
# @Preprocess -i $Input -o /tmp/tepapa-pl-preprocessed.txt CLEAN TOKENISE 

# Uncomment this line to use case-insensitive tokens instead:
@Preprocess -i $Input -o /tmp/tepapa-pl-preprocessed.txt CLEAN ANNOTATE-LC TOKENISE

# Uncomment this line to use character-based discovery (instead of word-based)
# @Preprocess -i $Input -o /tmp/tepapa-pl-preprocessed.txt CLEAN EXPLODE


@LoadCases /tmp/tepapa-pl-preprocessed.txt

#### Pattern search and PatWAS #########################################################
# Generating primary patterns (n-grams) by exhaustive n-gram search (maximum n = 10)
@DiscoverSymbolic  -n 10 -s 5           -O rt_symbolic

# Uncomment this line to generate primary patterns (n-grams) by exhaustive n-gram search 
# (maximum n = 10) with combinatorial search (this will enable word stemming).
# @DiscoverSymbolic  -n 10                -O rt_symbolic -d

@Transfer                -D rt_symbolic -O retval
@Transfer                -D rt_symbolic -O regextest

##########################################################################################
# Evaluating primary patterns (numeric) by exhaustive n-gram search (maximum n = 5) 
@DiscoverNumeric   -n 5                 -O rt_numeric 
@SigSelect               -D rt_numeric  -a $Siglevel
@Rank                    -D rt_numeric  -f $SortOrder
@Print                   -D rt_numeric 

#### Regular expression induction ########################################################
# Generating secondary meta-patterns (regular expressions)  
# Group features by positive/negative predictors (i.e. direction, "d") to avoid mixing 
#   positive and negative patterns

# Comment out the following two lines to disable regular expression use.
@SigSelect               -D regextest   -a $Siglevel
@GroupFeatures -f d      -D regextest 
@DiscoverRegex -g        -D regextest   -O retval -A 


#### Feature filtering and reduction #####################################################
@SigSelect               -D retval      -a $Siglevel
@GroupFeatures           -D retval      -f $FeatureGroupFmt
@ReduceFeatures          -D retval 

#### Sorting and printing feature by $SortOrder ###########################################
@Rank                    -D retval      -f $SortOrder
@Print                   -D retval 
@PrintTwoClassStats      -D retval
