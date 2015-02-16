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


void* consume(void* passedWQptr)
{
	struct workQueue* wQ = (struct workQueue*)passedWQptr;	//cast from void* to wQ*
	int numConsumed = 0;
	int numToConsume = totalWorkUnits;
	while(numConsumed <= numToConsume)		//produce this many work units
	{
		pthread_mutex_lock(&mutex);
		if(wQ->size > 0)
		{
			wQ->size--;
			char* copy = malloc(sizeof(char) * BUFFER_SIZE);
			int i;
			for(i = 0; i < BUFFER_SIZE; i++)
			{
				copy[i] = wQ->workUnits[wQ->size]->data[i];
			}
			//release mutex, notify condition variable?
			pthread_mutex_unlock(&mutex);
			pthread_cond_broadcast(&bufferFullStatus);

			//start consuming unit
			numConsumed++;
		}
		else
		{
			//unlock mutex
			pthread_mutex_unlock(&mutex);		//may be unnecessary
			//sleep and wait to be woken up
			pthread_cond_wait(&bufferEmptyStatus, &mutex);
		}
	}
}

void* produce(void* passedWQptr)	// params: pointer to work Queue
{
	struct workQueue* wQ = (struct workQueue*)passedWQptr;	//cast from void* to wQ*
	int numProduced = 0;
	int numToProduce = totalWorkUnits;
	while(numProduced <= numToProduce)		//produce this many work units
	{
		pthread_mutex_lock(&mutex);
		if(wQ->size < wQ->maxSize)
		{
			fprintf(stderr, "Added to \n");
			pthread_mutex_unlock(&mutex);
			//procuce something
			char* yolo = "yolo";
			//timer();
			//add to queue
			pthread_mutex_lock(&mutex);
			wQ->workUnits[wQ->size] = newWorkUnit(yolo);
			wQ->size++;
			//release mutex, notify condition variable?
			pthread_mutex_unlock(&mutex);
			pthread_cond_broadcast(&bufferEmptyStatus);
		}
		else
		{
			//unlock mutex
			pthread_mutex_unlock(&mutex);		//may be unnecessary
			//sleep and wait to be woken up
			pthread_cond_wait(&bufferEmptyStatus, &mutex);
		}
	}
}

int main(int argc, char* argv[])
{
	if(argc != 2)
	{
		fprintf(stderr, "Useage:\t./pc <number of consumers>\n");
		exit(1);
	}
	numConsumers = atoi(argv[1]);		//using atoi() here because no user will ever get to pass stupid args to this program
	if(numConsumers < 1)
	{
		fprintf(stderr, "You're going to need at least one consumers.\n");
		exit(1);
	}

	totalWorkUnits = numConsumers * UNITS_PER_CONSUMER;
	if(totalWorkUnits<100)
	{
		fprintf(stderr, "Something went wrong calculating how many work units to make. Please contact your system administrator.\n");
		exit(1);
	}
	//make work queue
	struct workQueue* workQ = newWorkQueue(QUEUE_SIZE);

	//initalise producer thread 	//may add multiple producers later...
	pthread_t producer;
	if(pthread_create(&producer, NULL, produce, workQ))
	{
		fprintf(stderr, "Error creating producer\n");
		return 1;
	}
	fprintf(stderr, "Started Procuder Thread #%i!\n", 0);


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

	//wait for producer to join
	if(pthread_join(consumers[i], NULL))
	{
		fprintf(stderr, "Error joining producer #%i\n", 0);
		return 2;
	}

	//waiting for consumers to join
	for(i = 0; i < numConsumers; i++)
	{
		if(pthread_join(consumers[i], NULL))
		{
			fprintf(stderr, "Error joining consumer thread #%i\n", i);
			return 2;
		}
	}

	return 0;
}
