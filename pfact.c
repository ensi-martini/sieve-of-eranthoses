#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <math.h>

#include "eratosthenes.h"


int main(int argc, char *argv[]) {

    // Turning off sigpipe
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR) {
        perror("signal");
        exit(1);
    }

    //Handle bad input
    if (argc != 2) {
    	fprintf(stderr, "Usage:\n\tpfact n\n");
    	exit(1);
    }

    char* ptr = NULL;

    int n = strtol(argv[1], &ptr, 10);

    if (strcmp(ptr, "") != 0 || n < 2) {
    	fprintf(stderr, "Usage:\n\tpfact n\n");
    	exit(1);
    }
   
   	//set initial pipes, m, and primes tracker
    int pid = 0;

    int * feeder = malloc(sizeof(int) * 2);
    pipe(feeder);

    int factors[2];
    int factor_count[2];
    int f_count;
    int f_val1;
    int f_val2;
    int filters;

    pipe(factors);
    pipe(factor_count);

    int zero = 0;

    if (write(factor_count[1], &zero, sizeof(int)) < 0) {
    	perror("write error on 54");
    	exit(2);
    }

    int m = 2; 
   	
  	int main_fork = fork();


  	if (main_fork > 0) { //main_fork parent

  		if (close(factor_count[1]) == -1) {
  			perror("close did not work properly 2");
  			exit(2);
  		}

  		if (close(factors[1]) == -1) {
  			perror("close did not work properly 3");
  			exit(2);
  		}

  		//write the initial ints to the pipe
    	for (int i = 2; i <= n; i++) {    

			if (write(feeder[1], &i, sizeof(int)) < 0) {
				perror("write error on 78");
				exit(2);
			}
		}

		if (close(feeder[0]) == -1) {
  			perror("close did not work properly 1");
  			exit(2);
  		}
		
		if (close(feeder[1]) == -1) {
  			perror("close did not work properly 4");
  			exit(2);
  		}

		//now let's work with the factors we get from downstream
		int all_status;

		if (wait(&all_status) < 0) {
			perror("wait error");
			exit(2);
		}

		else {

			if (WIFEXITED(all_status)) {

				if (WEXITSTATUS(all_status) != 255) {
		    		filters = WEXITSTATUS(all_status);
		    	}

		    	else { 
		    		exit(2);
		    	}	

				if (read(factor_count[0], &f_count, sizeof(int)) < 0) {
					perror("read error on 107");
					exit(2);
				}

				if (f_count == 0) {
					printf("%d is prime\n", n);
				}

				else if (f_count == 1) {

					if (read(factors[0], &f_val1, sizeof(int)) < 0) {
						perror("read error on 119");
						exit(2);
					}

					if (f_val1 == n) {
						printf("%d is prime\n", n);
					}

					else if (f_val1 * f_val1 == n) {
						printf("%d %d %d\n", n, f_val1, f_val1);
					}

					else if (n % f_val1 == 0 && (n / f_val1) % f_val1 == 0) {
						printf("%d is not the product of two primes\n", n);
					}

					else {
						printf("%d %d %d\n", n, f_val1, n / f_val1);
					}
				}

				else {

					if (read(factors[0], &f_val1, sizeof(int)) < 0) {
						perror("read error on 139");
						exit(2);
					}

					if (read(factors[0], &f_val2, sizeof(int)) < 0) {
						perror("read error on 144");
						exit(2);
					}


					if (f_val1 * f_val2 == n) {
						printf("%d %d %d\n", n, f_val1, f_val2);
					}

					else {
						printf("%d is not the product of two primes\n", n);
					}

				}

				if (close(factor_count[0]) == -1) {
					perror("close did not work properly 5");
		  			exit(2);
		  		}

		  		if (close(factors[0]) == -1) {
					perror("close did not work properly 6");
		  			exit(2);
		  		}

				printf("Number of filters = %d\n", filters);
			}
		}

		exit(0);
  	}
	
	else if (main_fork == 0) { //main_fork child

		while (m <= sqrt(n)) {
			
			if (close(feeder[1]) == -1) {
	  			perror("close did not work properly 7");
	  			exit(-1);
	  		}

			
			int * destination = malloc(sizeof(int) * 2); 
			
			
			if ((m * m == n) || (n % m == 0 && (n / m) % m == 0)) { //in either case we do not continue sieving

				if (write(factors[1], &m, sizeof(int)) < 0) {
					perror("write error on 178");
					exit(-1);
				}

				if (read(factor_count[0], &f_count, sizeof(int)) < 0) {
					perror("read error on 183");
					exit(-1);
				}
				
	    		f_count++;

				if (write(factor_count[1], &f_count, sizeof(int)) < 0) {
					perror("write error on 191");
					exit(-1);
				}

				if (close(factors[1]) == -1) {
					perror("close did not work properly 8");
		  			exit(-1);
				}

				if (close(factors[0]) == -1) {
					perror("close did not work properly 9");
		  			exit(-1);
				}

				if (close(factor_count[0]) == -1) {
					perror("close did not work properly 10");
		  			exit(-1);
				}

				if (close(factor_count[1]) == -1) {
					perror("close did not work properly 11");
		  			exit(-1);
				}

				if (close(feeder[0]) == -1) {
					perror("close did not work properly 12");
		  			exit(-1);
				}

		    	exit(0); //can exit with 0, as there is no more downstream sieves to be created and this sieve itself does not filter
		    }

		    if (pid == 0) { //forking the sieves, but each only being a child of the last
		    	pid = make_stage(m, feeder[0], &destination);
		    }

		    if (pid > 0) { //careful... there's the split here

		    	if (n % m == 0) { //there can still be a factor after this one

		    		if (write(factors[1], &m, sizeof(int)) < 0) {
						perror("write error on 210");
						exit(-1);
					}


					if (read(factor_count[0], &f_count, sizeof(int)) < 0) {
						perror("read error on 215");
						exit(-1);
					}
				
	    			f_count++;

					if (write(factor_count[1], &f_count, sizeof(int)) < 0) {
						perror("write error on 223");
						exit(-1);
					}

		    	}


				if (close(factors[1]) == -1) {
					perror("factors did not close properly");
					exit(-1);
				}

				if (close(factors[0]) == -1) {
					perror("factors did not close properly");
					exit(-1);
				}

				if (close(factor_count[1]) == -1) {
					perror("factor_count did not close properly");
					exit(-1);
				}

				if (close(factor_count[0]) == -1) {
					perror("factor_count did not close properly");
					exit(-1);
				}

				if (f_count >= 2) {
	    			exit(0);
	    		}

		    	int child_status;

		    	if (wait(&child_status) < 0) {
		    		perror("wait failed");
		    		exit(-1);
		    	}

		    	if (WEXITSTATUS(child_status) != 255) {
		    		exit(1 + WEXITSTATUS(child_status));		    		
		    	}

		    	else {
		    		exit(-1);
		    	}	
		    }

		    else if (pid == 0) {

		    	int x;

		    	if (read(destination[0], &x, sizeof(int)) < 0) { //getting the next m
		    		perror("read error on 262");
		    		exit(-1);
		    	}

		    	m = x;
		    	
		    	if (write(destination[1], &x, sizeof(int)) < 0) {//putting it back into sieve values
					perror("write error on 269");
					exit(-1);
				}

				free(feeder);
		    	feeder = destination;
		    }

		    else {
				perror("fork failed");
				exit(-1);
			}
		}
		
		if (n % m == 0) {

			if (write(factors[1], &m, sizeof(int)) < 0) {
				perror("write error on 286");
				exit(-1);
			}

			if (read(factor_count[0], &f_count, sizeof(int)) < 0) {
				perror("read error on 291");
				exit(-1);
			}
			
    		f_count++;

			if (write(factor_count[1], &f_count, sizeof(int)) < 0) {
				perror("write error on 298");
				exit(-1);
			}

			if (close(feeder[0]) == -1) {
					perror("close did not work properly 12");
		  			exit(-1);
				}

			if (close(factors[1]) == -1) {
				perror("factors did not close properly");
				exit(-1);
			}

			if (close(factors[0]) == -1) {
				perror("factors did not close properly");
				exit(-1);
			}

			if (close(factor_count[1]) == -1) {
				perror("factor_count did not close properly");
				exit(-1);
			}

			if (close(factor_count[0]) == -1) {
				perror("factor_count did not close properly");
				exit(-1);
			}

	    	exit(0); //can exit with 1, as there is no more downstream sieves to be created
		}

	}

	else {
		perror("fork failed");
		exit(2);
	}

	exit(0);
}