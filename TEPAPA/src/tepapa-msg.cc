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

#include "tepapa-msg.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include "strfriends.h"

// int msg_verbosity_level = VL_DEBUG; // VL_MILESTONE;
unsigned int msg_verbosity_level = VL_MILESTONE; // 

void msgf(unsigned int msglevel, const char* fmt, ...) {
	char buf[262144];
	va_list ap;
	va_start (ap, fmt);
	vsprintf (buf, fmt, ap);
	va_end (ap);
	
	const char* msglevel_str;

	if (msglevel > msg_verbosity_level) return;
	if (msg_verbosity_level == 0) return;
	
	string  msg = buf;
	
	switch(msglevel) {
		case VL_FATAL:     msglevel_str = "FATAL";      break;
		case VL_MILESTONE: msglevel_str = "MILESTONE";  break;
		case VL_NOTICE:    msglevel_str = "NOTICE";     break;
		case VL_ERROR:     msglevel_str = "ERROR";      break;
		case VL_WARNING:   msglevel_str = "WARNING";    break;
		case VL_INFO:      msglevel_str = "INFO";       break;
		case VL_DEBUG:     msglevel_str = "DEBUG";      break;
		default:           msglevel_str = "";           break;
		};

	switch(msglevel) {
		case VL_FATAL:     
		case VL_ERROR:     
		case VL_WARNING:   msg = msglevel_str + string(" ") + sub_program + string(" - ") + msg + string("\n"); break;
		case VL_NOTICE:    
		case VL_MILESTONE: 
		case VL_INFO:      
		case VL_DEBUG: break;
		default: break;
		};
	
	if (msglevel != VL_MILESTONE) {
		msg = strfmt ("[%8.3f] ", master_clock.elapsed_sec()) + msg;
		}
	
// 	fprintf(stderr, "%s", msg . c_str());
	fprintf(stdout, "%s", msg . c_str());
	fflush(stdout);
	
	if (msglevel == VL_FATAL) exit(1);
	}


// void fatal(const string& msg, bool show_usage ) {
// 	fprintf(stderr, "FATAL: %s: %s\n", sub_program, msg.c_str());
// 	exit(1);
// 	}
// 
// void warn(const string& msg) {
// 	fprintf(stderr, "WARNING: %s: %s\n", sub_program, msg.c_str());
// 	}
