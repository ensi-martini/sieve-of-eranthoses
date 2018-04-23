#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <math.h>

#include "eratosthenes.h"



pid_t make_stage(int m, int readfd, int** fds) {
	/*
	make_stage takes:
	    the filter value m,
	    a file descriptor readfd from which integers are recieved,
	    a pointer to an array of file descriptors suitable for use by pipe()

	The purpose of this function is to:
	    create a communication channel (pipe) between the current stage in the data pipeline and the next stage
	    create a process (fork) to set up the next stage or to print the final result
	    process the data (filter) for the current stage
	    clean up (close) any file descriptors that are no longer needed
	*/

	if (pipe(*fds) == -1) {
		perror("pipe failed");
		exit(-1);
	}

	//we must fork before we filter because otherwise the program will get hung up on a read

	int process = fork();

	if (process == 0) {

		if (close(readfd) == -1) {
			perror("readfd not closed properly");
			exit(-1);
		}
	}

	else if (process < 0) {
		perror("fork failed");
		exit(-1);
	}

	else if (process > 0) { //parent process


		if (filter(m, readfd, (*fds)[1]) == 1) {
			perror("filter did not work properly");
			exit(-1);
		}

		if (close(readfd) == -1) {
			perror("readfd not closed properly");
			exit(-1);
		}



			if (close((*fds)[0]) == -1) {
			perror("fds write not closed properly");
			exit(-1);
		}
		if (close((*fds)[1]) == -1) {
			perror("fds write not closed properly");
			exit(-1);
		}

	
	}

	return process;
}