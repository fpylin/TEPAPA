#ifndef __ngram_registry_h
#define __ngram_registry_h   1

#include "hashtable.h"
#include "bprof.h"

#include "config.h"

struct hash_registry_struct {
	bool            exists;
	hash_registry_struct () { exists = false; }
	};

struct ngram_pattern_stats {
	bool            exists;
	int             npos;
	binary_profile  bprof;
	
	ngram_pattern_stats() {
		exists = false;
		npos =0;
		}
	
	void set(int pnpos) {
		exists = true;
		npos = pnpos;
		}
	};

	
	
#if TEPAPA_MULTITHREAD
#include <thread>
#include <mutex>
#include <memory>
#include <map>
#endif  // TEPAPA_MULTITHREAD
	
template <typename T> class hash_registry: public hash_assoc_array<T> {
#if TEPAPA_MULTITHREAD
	std::mutex hash_registry_mutex;  // protects g_i
#endif  // TEPAPA_MULTITHREAD
	
	public:
	hash_registry() : hash_assoc_array<T>() {}
	
	hash_registry(const hash_table& ht) : hash_assoc_array<T>(ht) {}
	
	bool exists(hash_value hv) { 
#if TEPAPA_MULTITHREAD
		std::lock_guard<std::mutex> lock(hash_registry_mutex);
#endif
		if (hv >= hash_assoc_array<T>::size()) return false;
		return hash_assoc_array<T>::operator[](hv).exists ; 
		}
	
	void set_existence(hash_value hv) { 
#if TEPAPA_MULTITHREAD
		std::lock_guard<std::mutex> lock(hash_registry_mutex);
#endif
		hash_assoc_array<T>::operator[](hv).exists = true; 
		}

#if TEPAPA_MULTITHREAD

	std::mutex hash_registry_grab_mutex; 
	set<hash_value>   hv_processing;

	bool grab(hash_value hv) {
		while(1) {
			std::lock_guard<std::mutex> lock(hash_registry_grab_mutex);
			
			auto it = hv_processing.find( hv );
			if ( it == hv_processing.end() ) {
				hv_processing.insert(hv) ;
				break;
				}
			}
		
		if ( exists(hv) ) {
			std::lock_guard<std::mutex> lock(hash_registry_grab_mutex);
			auto it = hv_processing.find( hv );
			hv_processing.erase(it);
			return false ;
			}
		
		return true;
		}
	
	bool release(hash_value hv) {
		std::lock_guard<std::mutex> lock(hash_registry_grab_mutex);
		auto it = hv_processing.find( hv );
		if ( it != hv_processing.end() ) {
			hv_processing.erase(it);
			return true;
			}
		else {
			return false;
			}
		}

#endif
	};

typedef hash_registry<ngram_pattern_stats>  ngram_registry;
typedef hash_registry<hash_registry_struct> simple_hash_registry;



#endif // __ngramregistry_h
