#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "thread.h"
using namespace std;

void parent_thread(void *arg);
void child0_thread(void *arg);
void child1_thread(void *arg);

int main(int argc, char *argv[]){
	int x = 7;
    if (thread_create((thread_startfunc_t) parent_thread, &x) != -1){
        printf("ERROR: Thread library not initialized.\n");
    }
    if (thread_yield() != -1){
        printf("ERROR: Thread library not initialized.\n");
    }
    if (thread_lock(1) != -1){
        printf("ERROR: Thread library not initialized.\n");
    }
    if (thread_unlock(1) != -1){
        printf("ERROR: Thread library not initialized.\n");
    }
    if (thread_wait(1, 2) != -1){
        printf("ERROR: Thread library not initialized.\n");
    }
    if (thread_signal(1, 2) != -1){
        printf("ERROR: Thread library not initialized.\n");
    }
    if (thread_broadcast(1, 2) != -1){
        printf("ERROR: Thread library not initialized.\n");
    }
	thread_libinit((thread_startfunc_t) parent_thread, &x);
	
}
void parent_thread(void *arg){
	cout << "Parent thread: Started.\n";
	cout << "arg = " << * (int *) arg << endl;
    if (thread_libinit((thread_startfunc_t) child0_thread, arg) != -1){
        printf("ERROR: Thread library already initialized.\n");
    }
    if (thread_create((thread_startfunc_t) child0_thread, arg) == -1){
        printf("ERROR: child 0 thread failed to create");
    }
    else{
        printf("child0 thread created");
    }
	thread_create((thread_startfunc_t) child0_thread, arg);
    if (thread_yield() == -1){
        printf("ERROR: yield failed");
    }
    else{
        printf("Yield successful");
    }
	if (thread_create((thread_startfunc_t) child1_thread, arg) == -1){
        printf("ERROR: child1 thread failed to create");
    }
    else{
        printf("child1 thread created");
    }
	cout << "Parent thread: Finished.\n";
}
void child0_thread(void *arg){
	printf("Child0 thread: Started.\n");
    printf("Child0 thread: Finished.\n");
}
void child1_thread(void *arg){
	printf("Child1 thread: Started.\n");
    printf("Child1 thread: Finished.\n");
}