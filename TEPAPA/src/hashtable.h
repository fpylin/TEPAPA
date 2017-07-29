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

#ifndef __hashtable_h
#define __hashtable_h 1

#define DEBUG_HASHTABLE 0

typedef unsigned long hash_value ;

#include <stdlib.h>

using namespace std;

#include <map>
#include <string>
#include <vector>
#include <mutex>

#include "iptr.h"
#include "config.h"

#define HASH_TABLE_NUM_KEYS   0x1000

class hash_table {
	int                      hv_last;

	protected:
#if TEPAPA_MULTITHREAD
	std::mutex               hash_table_mutex;
#endif
	
	map<string, hash_value>  index[HASH_TABLE_NUM_KEYS];
	vector<string>           table;
	
	static hash_value hash_string(const char* str);
	
	public:
	hash_table();
	virtual ~hash_table();
	
	hash_value hash(const string& str);
	
	const char* lookup(hash_value hv)  { 
#if TEPAPA_MULTITHREAD
		std::lock_guard<std::mutex>  lock(hash_table_mutex);
#endif
		return table[hv].c_str(); 
		}
	const char* operator[](hash_value hv)  {
		return lookup(hv);
		}
	hash_value  operator()(const char* str) {
		return hash(str);
		}
	hash_value  operator()(const string& str) {
		return hash(str);
		}
	hash_value  operator()(char ch) {
		char str[2] = {ch, 0};
		return hash(str);
		}
	
	virtual void resize(int n) {
#if TEPAPA_MULTITHREAD
// 		fprintf(stderr, "%s", __PRETTY_FUNCTION__);
		std::lock_guard<std::mutex>  lock(hash_table_mutex);
#endif
		table.resize( max((unsigned int)(table.size()), ((unsigned int)n)) ); 
		}

#if TEPAPA_MULTITHREAD
	void locked_resize(int n) {
		table.resize( max((unsigned int)(table.size()), ((unsigned int)n)) ); 
		}
#endif

	const int size() const { return hv_last; } 
	
	void debug_print();
	};



#include "iptr.h"
#include <string>

template <class T>
	class hash_table_ex: public hash_table
	{
	vector< iptr<T> >   extra;

#if TEPAPA_MULTITHREAD
	std::mutex    hash_table_ex_mutex;
#endif
	
	void resize_extra(int n) {
#if TEPAPA_MULTITHREAD
// 		fprintf(stderr, "%s", __PRETTY_FUNCTION__);
		std::lock_guard<std::mutex>  lock(hash_table_ex_mutex);
#endif
		extra.resize(HASH_TABLE_NUM_KEYS);
		}
	
	public:
	hash_table_ex(): hash_table() {
		resize_extra(HASH_TABLE_NUM_KEYS);
		}

	virtual ~hash_table_ex() {}
	
	hash_value set(const string& name, T* data) {
		hash_value hv = hash(name);
		resize_extra( table.size() ) ;
		extra[hv]= data;
		return hv;
		}
	
	hash_value set(const char* name, const T& data) {
		hash_value hv = hash(name);
		resize_extra( table.size() ) ;
		extra[hv]= new T(data);
		return hv;
		}

// 	bool exists(hash_value hv) { return !!(extra[hv]); }
// 	bool exists(const char* p_name) { return !!(extra[hash(p_name)]); }
// 	bool exists(const string& p_name) { return !!(extra[hash(p_name)]); }
	
// 	const T& get(hash_value hv) const { return *(extra[hv]); }
// 	const T& get(const char* p_name) const { return *(extra[hash(p_name)]); }
// 	const T& get(const string& p_name) const { return *(extra[hash(p_name)]); }
	
// 	T& get(hash_value hv) { return *(extra[hv]); }
// 	T& get(const char* p_name) { return *(extra[hash(p_name)]); }
// 	T& get(const std::string& p_name) { return *(extra[hash(p_name)]); }
	
// 	const T& operator[](hash_value hv) const { return *(extra[hv]); }
// 	const T& operator[](const char* p_name) const { return *(extra[hash(p_name)]); }
// 	const T& operator[](const std::string& p_name) const { return *(extra[hash(p_name)]); }
	
// 	T& operator[](hash_value hv) { return *(extra[hv]); }
// 	T& operator[](const char* p_name) { return *(extra[hash(p_name)]); }
// 	T& operator[](const std::string& p_name) { return *(extra[hash(p_name)]); }
	
	const char* getname(hash_value hv) const { return hash_table::operator[](hv); }
	
	virtual void resize(int n) { 
		hash_table::resize(n);
		resize_extra( max((unsigned int)(extra.size()), (unsigned int)n) ); 
		}
	};



template <class T> class hash_assoc_array {
	const hash_table*  ht_ptr;
	vector<T>          data;
#if TEPAPA_MULTITHREAD
	static std::mutex  hash_assoc_array_mutex;
#endif
	
	void resize_vector_data() {
		data.resize( ht_ptr->size() );
		}
		
	public:
	hash_assoc_array() { 
		data.resize(HASH_TABLE_NUM_KEYS); 
		}
	
	void set_hash_table(const hash_table& ht_ref)  {
#if TEPAPA_MULTITHREAD
		std::lock_guard<std::mutex>  lock(hash_assoc_array_mutex);
#endif
		ht_ptr = & ht_ref;
		}
	
	hash_assoc_array(const hash_table& ht_ref) { 
		ht_ptr = & ht_ref;
		resize_vector_data(); 
		}
	
	virtual ~hash_assoc_array() {}
		
	T& operator[] (unsigned int i) {
#if TEPAPA_MULTITHREAD
		std::lock_guard<std::mutex>  lock(hash_assoc_array_mutex);
#endif
		if ( i >= data.size() ) resize_vector_data();
		return data[i];
		}
	
	const unsigned int size() const {
		return data.size();
		}
		
	};

template <class T> std::mutex  hash_assoc_array<T>::hash_assoc_array_mutex;


extern hash_table  ght;

	
#endif // #ifndef __hashtable_h
