/*
 *	Peter Meehan - 2015
 *	
 *	For info on what this code does and why, check out my github:
 *	https://github.com/22a/ConSys
 *
 */

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_RESET   "\x1b[0m"
// #define ANSI_COLOR_YELLOW  "\x1b[33m"
// #define ANSI_COLOR_BLUE    "\x1b[34m"
// #define ANSI_COLOR_MAGENTA "\x1b[35m"
// #define ANSI_COLOR_CYAN    "\x1b[36m"

#define UNITS_PER_CONSUMER 100
#define BUFFER_SIZE 2048
#define QUEUE_SIZE 50

#define sexyPrint(passedWQptr) {								\
	system("clear");											\
	int size = passedWQptr->size;								\
	printf("\n\n\n\tWork Queue Build Up:\n\t");					\
	int i;														\
	for(i = 0; i < QUEUE_SIZE; i++)								\
	{															\
		if(i!=0 && i%10 == 0)									\
		{														\
			printf(ANSI_COLOR_RESET "\n\t");					\
		}														\
		if(i < size)											\
		{														\
			printf(ANSI_COLOR_RED "[%.2i] ", i);				\
		}														\
		else													\
		{														\
			printf(ANSI_COLOR_GREEN "[  ] ");					\
		}														\
	}															\
	printf(ANSI_COLOR_RESET "\n");								\
}

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t bufferFullStatus = PTHREAD_COND_INITIALIZER;
pthread_cond_t bufferEmptyStatus = PTHREAD_COND_INITIALIZER;

int numConsumers;
int totalWorkUnits;

struct workUnit
{
	char* data;
};

struct workQueue
{
  int size;
  int maxSize;
  struct workUnit** workUnits;	//array of workUnits
};

struct workUnit* newWorkUnit(char* st)
{
	struct workUnit* newWU = malloc(sizeof(struct workUnit));
	newWU->data = st;
	return newWU;
}

struct workQueue* newWorkQueue(int maxSize)
{
	struct workQueue* newWQ = malloc(sizeof(struct workQueue));
	newWQ->size = 0;
	newWQ->maxSize = maxSize;
	newWQ->workUnits = malloc(sizeof(struct workUnit*) * maxSize);	//allocate size of 
	return newWQ;
}

void timer(int length)
{
	int i = length;
	while(i > 0)
	{
		i--;
	}
	return;
}

void* produce(void* passedWQptr)	// params: pointer to work Queue
{
	struct workQueue* wQ = (struct workQueue*)passedWQptr;	//cast from void* to wQ*
	int numProduced = 0;
	int numToProduce = totalWorkUnits;
	while(numProduced < numToProduce)		//produce this many work units
	{
		//procuce something, keep it locally
		char* data = "Peter_Meehan_2015";
		timer(10000000);			// this is done outside of mutex lock

		pthread_mutex_lock(&mutex);
		sexyPrint(wQ);
		printf("numProduced = %i\n", numProduced);
		if(wQ->size < wQ->maxSize)
		{
			//add stored unit to queue
			wQ->workUnits[wQ->size] = newWorkUnit(data);
			wQ->size++;
			numProduced++;
			//release mutex, notify condition variable
			pthread_mutex_unlock(&mutex);
			pthread_cond_broadcast(&bufferEmptyStatus);			//let consumers know the buffer isn't empty
		}
		else
		{
			pthread_mutex_unlock(&mutex);
			pthread_cond_wait(&bufferFullStatus, &mutex);	//wait until the queue has empty slots
		}
	}
	return NULL;
}

void* consume(void* passedWQptr)
{
	struct workQueue* wQ = (struct workQueue*)passedWQptr;	//cast from void* to wQ*
	int numConsumed = 0;
	int numToConsume = totalWorkUnits/numConsumers;
	while(numConsumed < numToConsume)		//produce this many work units
	{
		pthread_mutex_lock(&mutex);
		if(wQ->size > 0)
		{
			wQ->size--;
			// char* copy = malloc(sizeof(char) * BUFFER_SIZE);
			// int i;
			// for(i = 0; i < BUFFER_SIZE; i++)				//make copy of data recieved
			// {											//the array element is probably pretty volatile
			// 	copy[i] = wQ->workUnits[wQ->size]->data[i];
			// }

			//release mutex, notify condition variable?
			sexyPrint(wQ);
			pthread_mutex_unlock(&mutex);
			pthread_cond_broadcast(&bufferFullStatus);			//let producer know the buffer isn't full
			
			//start consuming unit
			numConsumed++;
			timer(40000000);
		}
		else
		{
			pthread_mutex_unlock(&mutex);
			pthread_cond_wait(&bufferEmptyStatus, &mutex);
		}
		pthread_mutex_unlock(&mutex);
	}
	return NULL;
}

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		fprintf(stderr, "Useage:\t./sexy_pc <number of consumers>\n");
		exit(1);
	}
	numConsumers = atoi(argv[1]);		//using atoi() here because no user will ever get to pass stupid args to this program
	if(numConsumers < 1 || numConsumers > 2048)
	{
		fprintf(stderr, "That amount of consumers isn't sensible. Take care.\n");
		exit(1);
	}
	totalWorkUnits = numConsumers * UNITS_PER_CONSUMER;
	if(totalWorkUnits<100)
	{
		//account for int overflow and other bs
		fprintf(stderr, "Something went wrong calculating how many work units to make. Please contact your system administrator.\n");
		exit(1);
	}

	fprintf(stderr, "Starting process:\nNumber of Consumers = %i\nTotal Work Units = %i\nUnits Per Consumer = %i\n\n", numConsumers, totalWorkUnits, UNITS_PER_CONSUMER);

	//make work queue
	struct workQueue* workQ = newWorkQueue(QUEUE_SIZE);

	//initalise producer thread 	//may add multiple producers later...
	pthread_t producer;
	if(pthread_create(&producer, NULL, produce, workQ))
	{
		fprintf(stderr, "Error creating producer\n");
		return 1;
	}
	fprintf(stderr, "Started Producer Thread #%i!\n", 0);

	pthread_cond_signal(&bufferFullStatus);	

	//pthread instance variables for consumers
	pthread_t consumers[numConsumers];
	int i;
	for(i = 0; i < numConsumers; i++)
	{
		if(pthread_create(&consumers[i], NULL, consume, workQ))
		{
			fprintf(stderr, "Error creating consumer #%i\n", i);
			return 1;
		}
		fprintf(stderr, "Started Consumer Thread #%i!\n", i);
	}
	fprintf(stderr, "\n" );

	//wait for producer to join
	if(pthread_join(producer, NULL))
	{
		fprintf(stderr, "Error joining producer #%i\n", 0);
		return 2;
	}
	fprintf(stderr, "Producer joined main thread successfully!\n");

	//waiting for consumers to join
	for(i = 0; i < numConsumers; i++)
	{
		if(pthread_join(consumers[i], NULL))
		{
			fprintf(stderr, "Error joining consumer thread #%i\n", i);
			return 2;
		}
		fprintf(stderr, "Consumer #%i joined main thread successfully!\n", i);
	}
	return 0;
}
