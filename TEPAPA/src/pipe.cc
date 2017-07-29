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
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>

using namespace std;

#include <string>

#include "pipe.h"
#include "strfriends.h"


string pipe_command (const string& progname, const string& data) {
	int fd_p2c[2], fd_c2p[2];
	
	string out = "";

	if ( pipe(fd_p2c) != 0 ) { perror("pipe"); fprintf(stderr, "Failed to create pipe 1."); exit(1); }
	if ( pipe(fd_c2p) != 0 ) { perror("pipe"); fprintf(stderr, "Failed to create pipe 2."); exit(1); }

	pid_t childpid = fork();

	if (childpid < 0) {
		perror("fork");
		fprintf(stderr, "Unable to fork.");
		exit(-1);
		}

	if (childpid == 0) { // is the child
		dup2(fd_p2c[0], 0) ; close(fd_p2c[0]) ; close(fd_p2c[1]) ;
		dup2(fd_c2p[1], 1) ; close(fd_c2p[1]) ; close(fd_c2p[0]) ;
		execl(progname.c_str(), progname.c_str(), (char *) 0);
		perror("execv"); 
		fprintf(stderr, "Unable to execute %s", progname.c_str() );
        exit(1);
		}
	
	// the parent process;
	
	close(fd_p2c[0]);
	close(fd_c2p[1]);

	int nbytes = data.length();
	
	if ( write(fd_p2c[1], data.c_str(), nbytes) != nbytes ) {
        fprintf(stderr, "Failed to write to |%s", progname.c_str() );
		exit(1);
		}
	
	close(fd_p2c[1]); 
	
	char buf[4096];
	while (1) {
		int size = read(fd_c2p[0], buf, sizeof(buf)-1);
		if (size<= 0) break;
		buf[size] = 0;
		out += buf;
		}
	
	close(fd_c2p[0]); 
	
    int status;
    waitpid(childpid, &status, 0);
	
	return out;
	}


string pipe_command (const string& progname, const vector<string>& p_argv, const string& data) {
	
	int fd_p2c[2], fd_c2p[2];
	
	string out = "";

	if ( pipe(fd_p2c) != 0 ) { perror("pipe"); fprintf(stderr, "Failed to create pipe 1."); exit(1); }
	if ( pipe(fd_c2p) != 0 ) { perror("pipe"); fprintf(stderr, "Failed to create pipe 2."); exit(1); }

// 	fprintf(stderr, "%s\n", __PRETTY_FUNCTION__);
	
	pid_t childpid = fork();

	if (childpid < 0) {
		perror("fork");
		fprintf(stderr, "Unable to fork.");
		exit(-1);
		}

	if (childpid == 0) { // is the child
		dup2(fd_p2c[0], 0) ; close(fd_p2c[0]) ; close(fd_p2c[1]) ;
		dup2(fd_c2p[1], 1) ; close(fd_c2p[1]) ; close(fd_c2p[0]) ;
		
		char* argv[1024];
		
		argv[0] = strdupa( progname.c_str() );
		for(uint i=0; i<p_argv.size(); ++i) {
			argv[i+1] = strdupa( p_argv[i].c_str() );
			}
		argv[ p_argv.size() + 1 ] = 0;
		
		execv(progname.c_str(), argv );
		
		perror("execv");
		fprintf(stderr, "Unable to execute %s", progname.c_str() );
        exit(1);
		}
	else 
		{ // the parent process;
		close(fd_p2c[0]);
		close(fd_c2p[1]);
	
		int nbytes = data.length();
		
		if ( write(fd_p2c[1], data.c_str(), nbytes) != nbytes ) {
			fprintf(stderr, "Failed to write to |%s", progname.c_str() );
			exit(1);
			}
		
		close(fd_p2c[1]); 
		
		char buf[4096];
		while (1) {
			int size = read(fd_c2p[0], buf, sizeof(buf)-1);
			if (size<= 0) break;
			buf[size] = 0;
			out += buf;
			}
		
		close(fd_c2p[0]); 
	
		int status;
		
// 		fprintf(stderr, "Waiting for %s (%d) to finish.\n", progname.c_str(), childpid);
		waitpid(childpid, &status, 0);
// 		fprintf(stderr, "Completed %s (%d).", progname.c_str(), childpid);
		}
		
	return out;
	}

