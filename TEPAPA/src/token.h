#ifndef __token_h
#define __token_h 1

#include <string.h>
#include <stdlib.h>

using namespace std;
#include <vector>
#include <algorithm>
#include <string>

#include "hashtable.h"

struct token: public vector<hash_value> {
 public:
 token(): vector<hash_value>() {} 
  token(const hash_value& hv) { 
    clear();
    push_back(hv);
  }
  
 token(const token& t): vector<hash_value>(t) { }
  
  hash_value primary()  const { return front(); }
  
  void print() const ;
  
  bool operator == (const token& t) const { return front() == t.front(); }
  
  bool has_class(const hash_value& hv) const { return find(begin(), end(), hv) != end(); };
};


#include <map>

class token_string_index {
  static vector<size_t>               null;
  map< hash_value, vector<size_t> >   index_table;
  
 public:
  token_string_index () {}
  virtual ~token_string_index () {}
  
  const vector<size_t>& find(hash_value hv) const ;
  int index(const class token_string& ts);
  void print(void) const;
};


class token_string: public vector<token>, public __iptr_member {
  token_string_index   tsi;
	
 public:
 token_string(): vector<token> (), __iptr_member() {}
 token_string(int n): vector<token> (n), __iptr_member() {}
  template <class InputIterator> token_string(InputIterator first, InputIterator last)
    : vector<token> (first, last), __iptr_member() {}
	
  void print() const ;
  
  int load_from_file(const char* filename) ;
  int load_charstring(const string& s) ;
  void index() { tsi.index(*this); }
  
  bool has_token(token h) const;
  vector<size_t>  find_pos(hash_value hv) const { return tsi.find(hv); }
};



	
#endif // #ifndef __token_h
