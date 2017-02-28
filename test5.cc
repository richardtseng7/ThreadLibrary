#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "thread.h"
using namespace std;

//test if different CVs work with one lock

void parent_thread(void *arg);
void child0_thread(void *arg);
void child1_thread(void *arg);

int main(int argc, char *argv[]){
    int x = 7;
    thread_libinit((thread_startfunc_t) parent_thread, &x);
}

void parent_thread(void *arg){
    cout << "Parent thread: Started.\n";
    cout << "arg = " << * (int *) arg << endl;
    thread_libinit((thread_startfunc_t) child0_thread, &x);
    thread_wait(1,1);
    cout << "Parent thread: Finished.\n";
}

void child0_thread(void *arg){
    printf("Child0 thread: Started.\n");
    if(thread_signal(1,2)==-1) {
        printf("Incorrect error handling.\n")
    }
    else {
        printf("CV2 mapped to lock.\n");
    }

    if(thread_signal(1,3)==-1) {
        printf("ERROR: Thread does not.\n")
    }
    else {
        printf("CV3 mapped to lock.\n");
    }

    thread_signal(1,3);
    printf("Child0 thread: Finished.\n");
}