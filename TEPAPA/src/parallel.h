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

#ifndef __parallel_h
#define __parallel_h 1

#include "config.h"

#if TEPAPA_MULTITHREAD 
#include <future>
#include <vector>
#include <functional>

#include "tepapa-msg.h"

class thread_manager: public vector< std::future<void> >  {
	unsigned int max_threads;
	
	public:
	thread_manager();
	~thread_manager();

	unsigned int get_max_threads() const { return max_threads; }
	void queue() ;
	void join_all() ;
// 	void push_back(const std::thread& t) {
// 		vector<std::thread>::push_back( t );
// 		}
// 	void create(const std::function& func);
	};

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#include <functional>

template <typename DATA_TYPE> void batch_process_group_const(const vector<DATA_TYPE>& data, std::function<void (const DATA_TYPE&)> lambda, unsigned int group_start, unsigned int group_end) {
	msgf(VL_DEBUG, "Processing groups %d to %d \n", group_start, group_end);
	
	for (unsigned int g=group_start; g<group_end; ++g) lambda(data[g]);
	}



template <typename DATA_TYPE> void batch_process_const(const vector<DATA_TYPE>& data, std::function<void (const DATA_TYPE&)> lambda, int unit_size=100) {
	thread_manager tm;

	for (unsigned int g=0; g<data.size(); g += unit_size) {
		tm.queue();
		unsigned int s = g;
		unsigned int e = g + unit_size;
		if (e > data.size()) e = data.size();
		tm.push_back(
			std::async( std::launch::async, &batch_process_group_const<DATA_TYPE>, std::ref(data), lambda, s, e )
			);
		}
	tm.join_all();
	}

template <typename DATA_TYPE> void batch_process_group(vector<DATA_TYPE>& data, std::function<void (DATA_TYPE&)> lambda, unsigned int group_start, unsigned int group_end) {
	msgf(VL_DEBUG, "Processing groups %d to %d \n", group_start, group_end);
	
	for (unsigned int g=group_start; g<group_end; ++g) lambda(data[g]);
	}

template <typename DATA_TYPE> void batch_process(vector<DATA_TYPE>& data, std::function<void (DATA_TYPE&)> lambda, int unit_size=100) {
	thread_manager tm;

	for (unsigned int g=0; g<data.size(); g += unit_size) {
		tm.queue();
		unsigned int s = g;
		unsigned int e = g + unit_size;
		if (e > data.size()) e = data.size();
		tm.push_back(
			std::async( std::launch::async, &batch_process_group<DATA_TYPE>, std::ref(data), lambda, s, e )
			);
		}
	tm.join_all();
	}




#endif // TEPAPA_MULTITHREAD
	
#endif // __parallel_h 1
