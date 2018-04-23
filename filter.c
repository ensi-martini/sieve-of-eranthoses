#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

int filter(int m, int readfd, int writefd) {
	/*
	Removes any integers from the data stream that are multiples of m. Takes:
	    the filter value m, 
	    a file descriptor readfd from which integers are received, 
	    and a file descriptor writefd to which integers are written. 
	The function returns 0 if it completes without encountering an error and 1 otherwise. 
	*/

	int num;
	int rerror = read(readfd, &num, sizeof(int));
	int werror;

	while (rerror > 0) {

		if (num % m != 0) {
			werror = write(writefd, &num, sizeof(int));

			if (werror < 0) {
				return 1;
			}
		}

		rerror = read(readfd, &num, sizeof(int));
	}

	if (rerror < 0) {
		return 1;
	}

	return 0;

}