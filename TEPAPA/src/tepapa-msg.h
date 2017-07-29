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

#ifndef __tepapa_msg_h
#define __tepapa_msg_h 1

// void fatal(const std::string& msg, bool show_usage = false);
// void warn(const std::string& msg) ;

#include "tepapa.h"

// Verbosity and log levels
#define VL_RESULT       1
#define VL_FATAL        2
#define VL_ERROR        3
#define VL_WARNING      4
#define VL_MILESTONE    5
#define VL_NOTICE       6
#define VL_INFO         7
#define VL_DEBUG        8

void msgf(unsigned int msglevel, const char* fmt, ...);

extern unsigned int msg_verbosity_level ;


#endif //  __tepapa_msg_h 
