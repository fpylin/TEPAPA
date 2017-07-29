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

#include "datatable.h"
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <stdio.h>

#include "tepapa-msg.h"

unsigned int const_map_get(const map<string,unsigned int>& m, const string& key) {
	auto it = m.find(key) ;
	if ( it != m.end() )  return it->second;
	msgf(VL_FATAL, "FATAL: name %s does not exist in header.\n", key.c_str());
	return 0;
	}

string& data_table::operator () (int r, int c) { return data[ r ][ c ]; }

const string& data_table::operator () (int r, int c) const {
	return data[ r ][ c ];
	}

string& data_table::operator () (int r, const string& c) {
	field_exists(ch, c);
	return data[ r ][ const_map_get(ch,c) ];
	}
	
const string& data_table::operator () (int r, const string& c) const {
	field_exists(ch, c);
	return data[ (ch.size()?r+1:r) ][ const_map_get(ch,c) ];
	}


void data_table::make_rectangular(void) {
	unsigned int maxcols = 0;
	for(unsigned int i=0; i<data.size(); ++i) if (maxcols < data[i].size()) maxcols = data[i].size();
	for(unsigned int i=0; i<data.size(); ++i) if (data[i].size() != maxcols) data[i].resize(maxcols);
	}


char* escape(char* out_buf, const char* in_buf, const char delimiter) {
	register const char *z ;
	register char *y=out_buf;
	bool should_quote = false;
	
	for (register const char* x = in_buf; *x; ++x) {
		if( isspace(*x) || (*x == delimiter)) {
			should_quote = true;
			break;
			}
		}
		
	if(should_quote) { *y = '\"'; ++y; }
	for(z=in_buf; *z; ++z, ++y) {
		switch(*z) {
			case '\n':  *y = '\\'; y++; *y = 'n';   break;
			case '\r':  *y = '\\'; y++; *y = 'r';   break;
			case '\t':  *y = '\\'; y++; *y = 't';   break;
			case '"':   *y = '\\'; y++; *y = '"';   break;
			case '\\':  *y = '\\'; y++; *y = '\\';  break;
			default: *y = *z;
			};
		}
	if(should_quote) { *y = '\"'; ++y; }
	*y =0;
	return y;
	}




char* unescape(char* buf, const char delim) {
	char *z, *y;
	for(z=buf, y=buf; *z; ++z, ++y) {
		*y = *z;
		if (*z == '\\') {
			++z;
			switch(*z) {
				case 'n':  *y = '\n';  break;
				case 'r':  *y = '\r';  break;
				case 't':  *y = '\t';  break;
				case '"':  *y = '"';   break;
				case '\'': *y = '\''; break;
				case '\\':  *y = '\\';  break;
				default:
					if (*z == delim) *y = delim;
					else --z;
				};
			}
		}
	*y =0;
	return z;
	}

const char* get_sanitised_string(char* buf, const char* start, const char* end, const char delim) {
	while (isspace(*start) && start < end) ++start;
	while (isspace(*end)   && start < end) --end;
	if ( (start!=end) && 
	      (*start=='"' || *start=='\'') &&
	      (*start==*end) ) // unquote
		{
		++start;
		--end;
		}
	strncpy(buf, start, end-start+1);
	buf[end-start+1] = 0;
	unescape(buf, delim);
	return buf;
	}

#define CSV_DATA_PARSE_IS_EOL(x) (((x)=='\n')||(!(x)))

vector<string>  data_table::parse_line(const char* buf, const char delim) {
	register bool last = 0;
	register const char *z = buf, *y = z;
	bool f_escape = 0;
	int  ch_quote = 0;
	char temp[65536];
	
	vector<string> retval;
	for (z=buf, y=z; !last; ++z) {
		switch(*z) {
			case '\r': continue; // ignored
			case '\\': ++z; continue;
			case '"':
			case '\'': if(ch_quote && *z == ch_quote) ch_quote = 0; else ch_quote = *z; continue;
			case 0:    last = 1;
			case '\n': if(! (*(z+1)) ) { last=1; ++z; }
			default:
				if ( !ch_quote && !f_escape && ( *z == delim || CSV_DATA_PARSE_IS_EOL(*z) ) ) {
					get_sanitised_string(temp, y, z-1, delim);
					retval.push_back(temp);
					y = z+1;
					if ( CSV_DATA_PARSE_IS_EOL(*z) ) break;
					}
				continue;
			};
		}
	
	return retval;
	}



bool data_table::save(const char* filename, const char delim) {
	char str[65536];
	register unsigned int i, j;
	FILE* fout = fopen (filename, "wt");
	if (!fout) return false;
	for(i=0; i<data.size(); ++i) {
		for(j=0; j<data[i].size(); ++j) {
			escape(str, data[i][j].c_str(), delim);
			fprintf(fout, "%s", str);
			if (j!=data[i].size()-1) fputc(delim, fout);
			}
		fputc('\n', fout);
		}
	fclose(fout);
	return true;
	}

bool data_table::load(const char* filename, const char delim) {
	data.clear();
	
	char line[1048576];
	
	FILE* file = fopen(filename, "rt");
	if(!file) {
		msgf(VL_FATAL, "Unable to open file %s", filename);
		}
	
	while( fgets(line, 1048576, file) ) {
		vector<string> new_row = parse_line(line, delim);
		if ( fields.size() == 0 ) {
			fields = new_row ;
			}
		else {
			data.push_back( new_row );
			}
		}
	
	fclose(file);
	
	return true;
	}

void data_table::init(void) {
	make_rectangular();
	for (unsigned int i=0; i<fields.size(); ++i ) ch[ fields[i] ]=i;
	get_field_levels();
	guess_field_types();
	}


void data_table::print(void) const {
	char str[65536];
	register unsigned int i, j;
	for(i=0; i<fields.size(); ++i) {
		printf( "%s%s", i==0 ? "" : "\t", fields[i].c_str() );
		}
	printf("\n");
	
	for(i=0; i<data.size(); ++i) {
		printf("%d: ", i);
		for(j=0; j<data[i].size(); ++j) {
			escape(str, data[i][j].c_str(), ',');
			printf("%s", str);
			if (j!=data[i].size()-1) printf("\t");
			}
		printf("\n");
		}
	}


void data_table::field_exists(const map<string, unsigned int>& s, const string& name) const {
	if (s.find(name) == s.end()) {
		msgf(VL_FATAL, "FATAL: name %s does not exist in header.\n", name.c_str());
		}
	}

vector<string> data_table::col(int c) const {
	vector<string>  retval;
	int nr = rows();
	for(int i=0; i<nr; ++i) retval.push_back( operator()(i,c) );
	return retval;
	}

vector<string> data_table::col(const string& c) const {
	vector<string>  retval;
	int nr = rows();
	for(int i=0; i<nr; ++i) retval.push_back( operator()(i,c) );
	return retval;
	}


vector<string> data_table::row(int r) const {
	vector<string>  retval;
	int nr = rows();
	for(int i=0; i<nr; ++i) retval.push_back( operator()(r,i) );
	return retval;
	}

const vector<string>& data_table::get_fields() const { return fields; }

#include "parse.h"
#include "stats.h"

void data_table::get_field_levels(void) {
	field_levels.resize( fields.size() );
	for(unsigned int i=0; i<fields.size(); ++i) {
		field_levels[i] = ::get_levels<string>( col(i) );
		}
	}

bool string_is_double(const string& s) {
	const char* z = s.c_str();
	if (! (z = parse_skip_whitespaces(z)) ) return false;
	if (! (z = parse_skip_double(z)) ) return false;
	if (! (z = parse_skip_whitespaces(z)) ) return false;
	if ( *z == 0 ) return true;
	return false;
	}
	
void data_table::guess_field_types(void) {
	field_type.resize( fields.size() ) ;
	for(unsigned int i=0; i<fields.size(); ++i) {
		int n_numeric = 0;
		int n_categoric = 0;
		int n_na = 0;
		for (unsigned int j=0; j<field_levels[i].size(); ++j) {
			if ( field_levels[i][j].length() == 0 ) {
				n_na ++;
				}
			else if ( string_is_double( field_levels[i][j] ) ) {
				n_numeric ++;
				}
			else {
				n_categoric ++;
				}
			}
		
		field_type[i] = ' ';
		
		if ( n_categoric > 0 ) 
			field_type[i] = 'c';
		else if ( n_numeric > 0 ) 
			field_type[i] = 'n';
		}
	}

const vector<string>& data_table::get_levels(const string& c) const {
	return field_levels[ const_map_get(ch, c) ] ;
	}

char data_table::get_field_type(const string& c) const {
	return field_type[ const_map_get(ch, c) ] ;
	}

void data_table::copy(const data_table& src, const vector<string>& columns) {
	data.clear();
	fields = columns;
	for (unsigned int r=0; r<src.rows(); ++r) {
		vector<string> row;
		for (unsigned int c=0; c<columns.size(); ++c) {
			row.push_back( src(r, columns[c]) );
			}
		data.push_back(row);
		}
	init();
	}

void data_table::transform_field_eq(const string& field, const string& value) {
	unsigned int ci = ch[field];
	
	fields[ci] = fields[ci] + "==" + value;
		
	for(unsigned int r=0; r<rows(); ++r) {
		data[r][ci] = ( (data[r][ci] == value) ? "1" : "0" );
		}
	}
