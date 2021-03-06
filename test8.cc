#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "thread.h"
using namespace std;

//Parent thread exited with lock, child0 is supposed to be stuck in lock q, program should exit

void parent_thread(void *arg);
void child0_thread(void *arg);

int main(int argc, char *argv[]){
    int x = 7;
    thread_libinit((thread_startfunc_t) parent_thread, &x);
}

void parent_thread(void *arg){
    cout << "Parent thread: Started.\n";
    cout << "arg = " << * (int *) arg << endl;  
    thread_lock(1);
    thread_create((thread_startfunc_t) child0_thread, arg);
    thread_yield();
    cout << "Parent thread: Finished.\n";
}

void child0_thread(void *arg){
    printf("Child0 thread: Started.\n");
    if (thread_lock(1) != -1){
        printf("ERROR: child0 thread should not be able to acquire lock.\n");
    }
    printf("ERROR: Child0 thread running.\n");
    thread_wait(1, 2);
    printf("ERROR: Child0 thread still running.\n");
}