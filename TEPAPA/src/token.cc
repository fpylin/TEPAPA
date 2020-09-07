#include "token.h"
#include <stdio.h>

#include "tepapa-msg.h"

void token::print() const {
	int cnt=0;
	for(const_iterator i=begin(); i!=end(); ++i, ++cnt) {
		printf("%s%s[%lu]", (( i != begin() ) ? "\t" : "") , ght[ *i ], *i);
		}
	}

void token_string::print() const {
	int cnt=0;
	for(const_iterator i=begin(); i!=end(); ++i, ++cnt) {
		printf("%d\t%lu\t%s", cnt, i->primary(), ght[ i->primary() ] );
		for (unsigned int j=1; j<i->size(); ++j) {
			printf("\t%s", ght[ (*i)[j] ] );
			}
		printf("\n");
		}
	}

int token_string::load_from_file(const char* filename) {
	char buf[1048576];
	FILE* fin = fopen (filename, "rt");
	if (!fin) {
		msgf(VL_FATAL, "Unable to load file %s\n", filename);
		return 0;
		}
	clear();

	while( fgets(buf, sizeof(buf), fin) ) {
		int len = strlen(buf);
		char* x=buf;
		x[len-1] = 0;
		
		char* y;
		
		token t ;
		
		while ( x && *x ) {
			if ( (y=strchr(x, '\t') ) ) {
				*y = 0;
				t.push_back( ght(x) );
				x = y+1;
				}
			else {
				t.push_back( ght(x) );
				break;
				}
			}
// 			t.print(ht); printf("\n");
		push_back(t);
		}
	fclose (fin);
	
	tsi.index(*this);
	
	return size();
	}


int token_string::load_charstring(const string& s) {
	clear();

	for(unsigned int i=0; i<s.length(); ++i) {
		char z = s[i];
		char buf[8] = { z, 0, 0, 0, 0, 0, 0 };
		string c(buf);
// 		putchar(z);
		token t;
		t.push_back( ght( c.c_str() ) );
		if ( isdigit(z) ) t.push_back( ght("[0-9]") );
// 		if ( isalpha(z) ) {
// 		    sprintf(buf, "[%c%c]", toupper(z), tolower(z) );
// 		    t.push_back( ght( buf ) );
// 		 }
		// if ( isupper(buf[0]) ) t.push_back( ght("[[:upper:]]") );
		// if ( islower(buf[0]) ) t.push_back( ght("[[:lower:]]") );
		//if ( isalnum(buf[0]) ) t.push_back( ght("[[:alnum:]]") );
// 		if ( ispunct(z) ) t.push_back( ght("[[:punct:]]") );
// 		if ( isspace(z) ) t.push_back( ght("[[:space:]]") );  
		if ( ispunct(z) || isspace(z) ) t.push_back( ght("[[:punct:][:space:]]") );
// 		if ( ) t.push_back( ght("[[:space:]]") );  
// 		t.push_back( ght(".") );  
		push_back(t);
		}
// 	putchar('\n');
// 	to_string();
	tsi.index(*this);
	return size();
	}


bool token_string::has_token(token h) const {
	for (const_iterator i=begin(); i!=end(); ++i) {
		if ( *i == h ) return true;
		}
	return false;
	}



//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

vector<size_t> token_string_index::null;

int token_string_index::index(const token_string& ts) {
  index_table.clear();
  register int n = 0;
  for(size_t i=0; i<ts.size(); ++i) {
    const token& t = ts[i];
    for(size_t j=0; j<t.size(); ++j) {
      register hash_value h = t[j];
      index_table[h].push_back(i);
      ++n;
    }
  }
  return n;
}

void token_string_index::print(void) const {
  map< hash_value, vector<size_t> >::const_iterator i;
  for (i = index_table.begin(); i!= index_table.end(); ++i) {
    msgf(VL_DEBUG, "%lu:", i->first);
    for(unsigned int j=0; j<i->second.size(); ++j) {
      msgf(VL_DEBUG, " %d", i->second[j]);
    }
    msgf(VL_DEBUG, "\n");
  }
}

const vector<size_t>& token_string_index::find(hash_value hv) const {
  map< hash_value, vector<size_t> >::const_iterator i = index_table.find(hv);
  if (i == index_table.end()) return null;
  return i->second;
}
