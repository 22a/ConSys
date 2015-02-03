#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define MIN_ITER_PER_THREAD 2

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
int iter;
int numThreads;
double pi;

/* this function is run by the threads */
void *calc(void* i_ptr)
{
	int i = *((int*)i_ptr);
	int parity = (i+1)%2==0?1:-1;
	int jump = numThreads*2;
	double val = 0;
	for(i = ((i+1)*2)+1; i < iter; i+=jump)
	{
		val+=(4.0/(double)i)*(double)parity;
	}
	//printf("new thread finished, total = %f\n", val);
	pthread_mutex_lock(&mutex);
	pi+=val;
	pthread_mutex_unlock(&mutex);
	/* the function must return something */
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
	numThreads = (int)atol(argv[1]);
	iter = (int)atol(argv[2]);
	if(numThreads > iter*MIN_ITER_PER_THREAD)
	{
		fprintf(stderr, "You don't need %i threads to execute %i iterations! Make sure you have at least %i iterations per thread\n", numThreads, iter, MIN_ITER_PER_THREAD);
		exit(1);
	}
	if(numThreads<1)
	{
		fprintf(stderr, "You're going to need at least one thread.\n");
		exit(1);
	}

	if(numThreads==1)
	{
		int i;
		int parity = -1;
		pi = 4.0;
		for( i = 3; i < iter; i+=2)
		{
			pi+=((double)parity)*(4.0/(double)i);
			parity = parity==1?-1:1;
		}
		printf("pi = %f\n", pi);
	}
	else
	{
		pthread_t threads[numThreads];
		pi = 4.0;
		int i;
		int is[numThreads];
		for(i = 0; i < numThreads; i++)
		{
			is[i] = i;
			if(pthread_create(&threads[i], NULL, calc, &is[i]))
			{
				fprintf(stderr, "Error creating thread #%i\n", i);
				return 1;
			}
		}

		for(i = 0; i < numThreads; i++)
		{
			if(pthread_join(threads[i], NULL))
			{
				fprintf(stderr, "Error joining thread #%i\n", i);
				return 2;
			}
		}
		printf("pi = %f\n", pi);
	}

	// end = clock();
	// time_spent = (double)(end - begin) / CLOCKS_PER_SEC;
	// fprintf(stderr, "Time spent: %f\n", time_spent);

	return 0;
}