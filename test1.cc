#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "thread.h"
using namespace std;

void thread0(void *arg);

int main(int argc, char *argv[]){
    int x = 7;
    thread_libinit((thread_startfunc_t) thread0, &x);
    
}
void thread0(void *arg){
    cout << "Thread0: Started.\n";
    cout << "arg = " << * (int *) arg << endl;
    if (thread_lock(1) == 0){
        printf("Thread0: Acquired lock.\n");
    }
    else{
        printf("ERROR: Thread0 Failed to acquire lock.\n");
    }
    if (thread_lock(1) != -1){
        printf("ERROR: Lock has already been acquired by this thread.\n");
    }
    else{
        printf("Correct error handling for acquiring lock already held.\n");
    }
    if (thread_unlock(2) != -1){
        printf("ERROR: Thread never acquired this lock.\n");
    }
    else{
        printf("Correct error handling for unlock.\n");
    }
    if (thread_unlock(1) == 0){
        printf("Thread0: Unlocked.\n");
    }
    else{
        printf("ERROR: Thread0 failed to unlock.\n");
    }
    if (thread_unlock(1) != -1){
        printf("ERROR: Thread has already unlocked this lock.\n");
    }
    else{
        printf("Correct error handling for unlocking already unlocked lock.\n");
    }
    cout << "Thread0: Finished.\n";
}