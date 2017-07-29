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
#include "utils.h"
#include "tepapa-msg.h"




#include <sys/stat.h>

bool file_exists (const string& path) {
	struct stat st;   
	return (stat (path.c_str(), &st) == 0); 
	}

bool dir_exists (const string& path) {
	struct stat st;   
    if( stat( path.c_str(), &st ) != 0 ) return false;
    if(st.st_mode & S_IFDIR) return true;
    return false;
	}

#include <string.h>
#include <libgen.h>

string dirname_r(const string& path) {
	char buf[ path.size() + 1 ];
	strcpy( buf, path.c_str() );
	char* z = dirname( buf );
	return string(z); 
	}

string basename_r(const string& path) {
	char buf[ path.size() + 1 ];
	strcpy( buf, path.c_str() );
	char* z = basename( buf );
	return string(z); 
	}


#include <unistd.h>
#include <sys/time.h>
time_t mtime(const string& path) {
	struct stat st;   
    if( stat( path.c_str(), &st ) != 0 ) return 0;
	return st.st_mtim.tv_sec ;
	}

size_t filesize(const string& path) {
	struct stat st;   
    if( stat( path.c_str(), &st ) != 0 ) return 0;
	return st.st_size ;
	}

	

#include <fstream>
#include <streambuf>

string load_file(const string& path) {
	unsigned int size = filesize( path.c_str() );
// 	fprintf(stderr, "%lu\t%s\n", size, path.c_str() );
	char buf[size + 1];
	
	FILE* fin = fopen( path.c_str(), "rt" );
	if (!fin) {
		perror("fopen");
		msgf(VL_FATAL, "Unable to open file %s", path.c_str()) ;
		}
	
	if ( fread (buf, 1, size, fin) <= 0 ) {
		perror("fread");
		msgf(VL_FATAL, "Unable to read from file %s", path.c_str()) ;
		}
	buf[size] = 0;
	fclose (fin);
	
	string str = buf;
	
	return str;
	}

bool save_file(const string& path, const string& data) {
	unsigned int size = data.size();
	
	FILE* fout = fopen( path.c_str(), "wt" );
	
	if (!fout) {
		perror("fopen");
		msgf(VL_FATAL, "Unable to open file %s for writing", path.c_str()) ;
		}
	
	if ( fwrite (data.c_str(), 1, size, fout) <= 0 ) {
		perror("fwrite");
		msgf(VL_FATAL, "Unable to write to file %s", path.c_str()) ;
		}

	fclose (fout);
	
	return true;
	}
