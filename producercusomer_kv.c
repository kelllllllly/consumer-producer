
//#include "buffer.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>

typedef int buffer_item;
#define BUFFER_SIZE 5
#define TRUE 1

int count; 

// sempahores 
sem_t emptySpace;

sem_t fullSpace; 

pthread_mutex_t mutex;

// the buffer
buffer_item buffer[BUFFER_SIZE];


// inserts an item into the buffer
int insert_item(buffer_item item){
    
    if(count < BUFFER_SIZE){
        buffer[count] = item;
        count++;
        return 0;
    }
    else{
        return -1; 
    }
}

// removes an item from the buffer
int remove_item(buffer_item *item){
    
    if(count > 0){
        *item = buffer[--count];
        return 0;
    }
    else{
        return -1; 
    }
}

void *producer(void *param){
    buffer_item item;

    while (TRUE){
        // sleep randomly between 1 and 10 seconds 
        sleep(rand()% 10 + 1);

        // create an item to be inserted 
        item = rand();

        //decrement empty semaphore 
        sem_wait(&emptySpace);
        // acquire mutex lock 
        pthread_mutex_lock(&mutex);
        
        // critical selection 
        if(insert_item(item) == 0){
            printf("producer inserted %d\n", item);
        }
        else{
            printf("producer error.\n");
        }
        
        // release mutex lock 
        pthread_mutex_unlock(&mutex);

        // consumer process can now start
        sem_post(&fullSpace);

    }
}

void *consumer(void *param){
    buffer_item item;

    while(TRUE){
        // sleep randomly between 1 and 10 seconds 
        sleep(rand()% 10 + 1);
            
        //decrement empty semaphore 
        sem_wait(&fullSpace);
        
        // acquire mutex lock 
        pthread_mutex_lock(&mutex);
        
        // critical selection 
        if(insert_item(&item) == 0){
            printf("consumer removed %d\n", item);
        }
        else{
            printf("consumer error.\n");
        }
        
        // release mutex lock 
        pthread_mutex_unlock(&mutex);

        // consumer process can now start
        sem_post(&emptySpace);

    }
}

void init_locks() {

    // Initialize mutex lock
    pthread_mutex_init(&mutex, NULL);

    // Initialize full semaphore to 0 and empty semaphore to BUFFER_SIZE
    sem_init(&fullSpace, 0, 0);
    sem_init(&emptySpace, 0, BUFFER_SIZE);
}
int main(int argc, char *argv[]){
    
    // get cmd line args
    int sleep_time = atoi(argv[1]);
    int producer_count = atoi(argv[2]);
    int consumer_count = atoi(argv[3]);

    // initalize the buffer count 
    count = 0;
    
    init_locks();

    // create producer and consumer threads
    int i = 0;
    pthread_t thread_ID; 

    for (i=0; i<producer_count; i++) {
        pthread_create(&thread_ID, NULL, producer, NULL);
    }

    for (i=0; i<consumer_count; i++) {
        pthread_create(&thread_ID, NULL, consumer, NULL);
    }

    // terminate 
    sleep(sleep_time);
    printf("done.");
    exit(0);
    
    return 0;
}