#include <iostream>
#include <fstream>
#include <stdlib.h>
#include "thread.h"
#include <map>
#include <math.h>
#include <vector>
#include <string>
#include <stdio.h>
#include <inttypes.h>
#include <limits.h>

//check orders 

using namespace std;
int numSodas;
int consumers;
int maxSodas = 5;
unsigned int sodaLock = 2000;
unsigned int hasRoom = 3000;
unsigned int hasSoda = 4000;


void producer(void* arg){
	thread_lock(sodaLock);
	while (consumers>0) {
		while (numSodas == maxSodas) {
			printf("producer waiting... numsodas: %d \n",numSodas);
			thread_wait(sodaLock,hasRoom);
		}
	numSodas+=12;
	thread_yield();
	thread_broadcast(sodaLock,hasSoda);
	printf("producer broadcast numsodas: %d \n",numSodas);
	thread_unlock(sodaLock);
	printf("producer unlock numsodas: %d \n",numSodas);
	}
}


void consumer(void* arg){
	int cashierID = (intptr_t) arg;
	thread_lock(sodaLock);
	while (numSodas==0) {
		thread_wait(sodaLock,hasSoda);
		printf("consumer %d waiting numsodas: %d \n",cashierID,numSodas);
	}
	numSodas--;
	thread_signal(sodaLock,hasRoom);
	printf("consumer %d signal numsodas: %d \n",cashierID,numSodas);
	consumers--;
	thread_unlock(sodaLock);
	printf("consumer %d unlock numsodas: %d \n",cashierID,numSodas);
}


void init(void* arg){
    //start_preemptions(false, true, 1);
	// initialize sandwich maker thread
	numSodas = 0;
	consumers =105;
	// initialize all cashiers threads
	for (int i = 0; i < consumers; i++){ 
		printf("init consumer %d\n",i);
		thread_create((thread_startfunc_t) consumer, (void *) (intptr_t) i);
	}
	thread_create((thread_startfunc_t) producer, (void *) 14);
}


int main(int argc, char* argv[]) {
	
	thread_libinit((thread_startfunc_t) init, (void *) 7);
}