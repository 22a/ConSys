#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

int numThreads;

void* consume(void* passed_ptr)
{
	return;
}

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		fprintf(stderr, "Useage:\t./pc <number of threads>\n");
		exit(1);
	}
	numThreads = atoi(argv[1]);		//using atoi() here because no user will ever get to pass stupid args to this program

	//sanity checks to stop 2billion threads being created by accident
	if(numThreads<1)
	{
		fprintf(stderr, "You're going to need at least one thread.\n");
		exit(1);
	}
	int i;
	pthread_t threads[numThreads];	//pthread instance variables
	//thread creation
	for(i = 0; i < numThreads; i++)
	{
		if(pthread_create(&threads[i], NULL, consume, &i))
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

	return 0;
}