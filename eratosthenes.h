#include <unistd.h>

#ifndef ERAT_H
#define ERAT_H

int filter(int m, int readfd, int writefd);

// fds is a pointer to an array of 2 ints, suitable for using with pipe()
pid_t make_stage(int m, int read_fd, int **fds);

#endif

//we want to find if N can be made as the product of two primes
//we generate a list of numbers from 2-m, where m = n
//the filters we create can stop if we reach floor(sqrt(n)) by mathematical property

//