#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "thread.h"
using namespace std;


static int count;
void master_thread(void* args);
void child_thread(void* args);
void child2_thread(void*args);

int main(int argc, char *argv[]){
	int n = 7;
	count = 0;
	thread_libinit((thread_startfunc_t) master_thread, &n);
	printf("count: %d",count);
}

void counting(void * args){
	thread_lock(1);
	for (int i=0;i<100;i++) {
		count++;
	}
	thread_unlock(1);
}

void counting2(void *args){
	thread_lock(1);
	for (int i=0;i<100;i++) {
		count++;
	}
	thread_unlock(1);
}

void master_thread(void * args){
	cout << "master thread started\n";
	thread_create((thread_startfunc_t) counting, args);
	thread_create((thread_startfunc_t) counting2, args);
	cout << "master thread ended.\n";
}
