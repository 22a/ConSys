#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
//#include <time.h>

#define MIN_ITER_PER_THREAD 2

// Peter Meehan - 2015
// check https://github.com/22a/ConSys/tree/master/lab1 for info on what this terrible code is for

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int iter;
int numThreads;
double pi;

/* this function is run by the threads */
void *calc(void* i_ptr)
{
	int i = *((int*)i_ptr);				//void* to int casting magic
	double parity = (double)((i+1)%2==0?1:-1);
	int jump = numThreads*2;
	double val = 0;
	for(i = ((i+1)*2)+1; i < iter; i+=jump)
	{
		val+=(4.0/(double)i)*parity;
	}

	//politely wait for mutex lock
	pthread_mutex_lock(&mutex);
	//"my precious" global variable
	pi+=val;
	pthread_mutex_unlock(&mutex);
	return NULL;
}

int main(int argc, char* argv[])
{
	// clock_t begin, end;
	// double time_spent;
	// begin = clock();


	if(argc != 3)
	{
		fprintf(stderr, "Useage:\t./pi <number of threads> <number of iterations>\n");
		exit(1);
	}
	numThreads = atoi(argv[1]);		//using atoi() here because no user will ever get to pass stupid args to this program
	iter = atoi(argv[2]);			//only nice, clean, sensible scripts

	//sanity checks to stop 2billion threads being created by accident
	if(numThreads > iter*MIN_ITER_PER_THREAD)
	{
		fprintf(stderr, "You don't need %i threads to execute %i iterations! Make sure you have at least %i iterations per thread\n", numThreads, iter, MIN_ITER_PER_THREAD);
		exit(1);
	}
	else if(numThreads<1)
	{
		fprintf(stderr, "You're going to need at least one thread.\n");
		exit(1);
	}

	//iterative method for for when only one thread is selected, get rid of thread creation overhead entirely
	if(numThreads==1)
	{
		int i;
		double parity = -1.0;
		pi = 4.0;
		for( i = 3; i < iter; i+=2)
		{
			pi+=(parity)*(4.0/(double)i);
			parity = parity==1?-1:1;
		}
	}
	else
	{
		pthread_t threads[numThreads];	//pthread instance variables
		pi = 4.0;
		int i;
		int is[numThreads];	//this array is needed as non volatile storage of the for loop incrementer

		//thread creation
		for(i = 0; i < numThreads; i++)
		{
			is[i] = i;
			if(pthread_create(&threads[i], NULL, calc, &is[i]))
			{
				fprintf(stderr, "Error creating thread #%i\n", i);
				return 1;
			}
		}

		//waiting for threads to join
		for(i = 0; i < numThreads; i++)
		{
			if(pthread_join(threads[i], NULL))
			{
				fprintf(stderr, "Error joining thread #%i\n", i);
				return 2;
			}
		}
	}
	//printf("pi = %f\n", pi);

	// end = clock();
	// time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	// fprintf(stderr, "Time spent: %f\n", time_spent);

	return 0;
}