#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define UNITS_PER_CONSUMER 100
#define BUFFER_SIZE 2048
#define QUEUE_SIZE 50

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
	while(numProduced <= numToProduce)		//produce this many work units
	{
		//procuce something, keep it locally
		timer(1000000);
		char* yolo = "yolo";
		pthread_mutex_lock(&mutex);
		if(wQ->size < wQ->maxSize)
		{
			//add stored unit to queue	
			fprintf(stderr, "PRODUCER +++ Added unit number %i to the queue at position %i!\n", numProduced, wQ->size);
			wQ->workUnits[wQ->size] = newWorkUnit(yolo);
			wQ->size++;
			numProduced++;
			//release mutex, notify condition variable
			pthread_mutex_unlock(&mutex);
			pthread_cond_broadcast(&bufferEmptyStatus);			//let consumers know the buffer isn't empty
			
		}
		else
		{
			fprintf(stderr, "~~~~ QUEUE IS FULL, PRODUCER WAITING ~~~~\n");

			pthread_mutex_unlock(&mutex);
			pthread_cond_wait(&bufferFullStatus, &mutex);	//wait until the queue has empty slots
			
			fprintf(stderr, "~~~~ QUEUE NO LONGER FULL, PRODUCER RESUMING ~~~~\n");
		}

	}
	fprintf(stderr, "Exiting producer thread\n");
	return NULL;
}

void* consume(void* passedWQptr)
{
	struct workQueue* wQ = (struct workQueue*)passedWQptr;	//cast from void* to wQ*
	int numConsumed = 0;
	int numToConsume = totalWorkUnits/numConsumers;
	while(numConsumed < numToConsume)		//produce this many work units
	{
		fprintf(stderr, "\t\t\t\t\t\t\t\tconsumed/toConsume: %i/%i\n", numConsumed, numToConsume);
		pthread_cond_wait(&bufferEmptyStatus, &mutex);
		if(wQ->size > 0)
		{
			wQ->size--;
			fprintf(stderr, "CONSUMER --- Consuming block in place number %i\n", wQ->size);

			// char* copy = malloc(sizeof(char) * BUFFER_SIZE);
			// int i;
			// for(i = 0; i < BUFFER_SIZE; i++)
			// {
			// 	copy[i] = wQ->workUnits[wQ->size]->data[i];
			// }

			//release mutex, notify condition variable?
			pthread_mutex_unlock(&mutex);
			pthread_cond_signal(&bufferFullStatus);			//let producer know the buffer isn't full
			
			//start consuming unit
			numConsumed++;
			timer(10000);
		}
		else
		{
			fprintf(stderr, "~~~~ QUEUE IS EMPTY, CONSUMER WAITING ~~~~\n");
			pthread_mutex_unlock(&mutex);
			pthread_cond_wait(&bufferEmptyStatus, &mutex);
			fprintf(stderr, "~~~~ QUEUE NO LONGER EMPTY, CONSUMER RESUMING ~~~~\n");
		}
	}
	fprintf(stderr, "Exiting consumer thread\n");
	return NULL;
}

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		fprintf(stderr, "Useage:\t./pc <number of consumers>\n");
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
