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
#include <unistd.h>
#include <chrono>
#include "parallel.h"

thread_manager::thread_manager(): vector< std::future<void> >() {
	max_threads = std::thread::hardware_concurrency();

	msgf(VL_NOTICE, "Maximum of %d threads supported\n", max_threads);
	
// 	resize(max_threads);
	}

thread_manager::~thread_manager(){
	join_all();
	}

void thread_manager::queue() {
	while ( size() >= max_threads ) {
		bool flag = 0;
		iterator z= begin() ;
		while( z != end() ) {
			std::future_status status = z -> wait_for( std::chrono::seconds(0) );
			if (status == std::future_status::timeout) {
				++z;
				}
			else {
				z = erase( z );
				flag = 1;
				}
			}
		if (! flag) usleep(10);
		}
	}

// void thread_manager::join_all() {
// 	while ( size() > 0) {
// 		bool flag = 0;
// 		iterator z= begin() ;
// 		while( z != end() ) {
// 			std::future_status status = z -> wait_for( std::chrono::seconds(0) );
// 			if (status == std::future_status::timeout) {
// 				++z;
// 				}
// 			else {
// 				z = erase( z );
// 				flag = 1;
// 				}
// 			}
// 		if (! flag) usleep(10);
// 		}
// 	}
// 	
void thread_manager::join_all() {
// 	fprintf( stderr,"%lu threads left.\n", size() );
	while ( size() > 0 )  {
		iterator i=begin(); 
		while( i -> wait_for( std::chrono::seconds(1) ) == std::future_status::timeout) {
			}
		i -> get();
		i = erase(i);
// 		i -> wait();
		}
// 	/*usleep*/(100);
// 	fprintf( stderr,"%lu threads left.\n", size() );
	}

