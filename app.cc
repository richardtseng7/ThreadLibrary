#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "thread.h"
using namespace std;



void master_thread(void* args);
void child_thread(void* args);
void child2_thread(void*args);

int main(int argc, char *argv[]){
	int n = 7;
	thread_libinit((thread_startfunc_t) master_thread, &n);
	
}
void master_thread(void * args){
	cout << "master thread started\n";
	cout << "number is " << * (int *) args << endl;
	thread_create((thread_startfunc_t) child_thread, args);
	//thread_yield();
	thread_create((thread_startfunc_t) child2_thread, args);
	cout << "master thread ended.\n";
}
void child_thread(void * args){
	printf("child thread started\n");
	
}
void child2_thread(void * args){
	printf("child 2 thread started\n");
	
}