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
void child1_thread(void *arg);
void child2_thread(void *arg);
void child3_thread(void *arg);
void child4_thread(void *arg);
void child5_thread(void *arg);


int main(int argc, char *argv[]){
    int x = 7;
    thread_libinit((thread_startfunc_t) parent_thread, &x);
}

void parent_thread(void *arg){
    cout << "Parent thread: Started.\n";
    cout << "arg = " << * (int *) arg << endl;  
    thread_lock(1);
    thread_create((thread_startfunc_t) child5_thread, arg);
    thread_create((thread_startfunc_t) child3_thread, arg);
    thread_create((thread_startfunc_t) child2_thread, arg);
    thread_yield();
    printf("Thread returns from yield.\n");
    thread_create((thread_startfunc_t) child0_thread, arg);
    thread_create((thread_startfunc_t) child1_thread, arg);
    thread_create((thread_startfunc_t) child4_thread, arg);
    thread_unlock(1);
    cout << "Parent thread: Finished.\n";
}

void child0_thread(void *arg){
    printf("Child0 thread: Started.\n");
    if (thread_lock(1) != 0){
        printf("ERROR: Child 0 could not acquire lock.\n");
    }
    printf("Child0 thread running.\n");
    if (thread_unlock(1) != 0){
        printf("ERROR: Child 0 could not unlock.\n");
    }
}

void child1_thread(void *arg){
    printf("Child1 thread: Started.\n");
    if (thread_lock(1) != 0){
        printf("ERROR: Child 1 Could not acquire lock.\n");
    }
    printf("Child1 thread running.\n");
    if (thread_unlock(1) != 0){
        printf("ERROR: Child 1 Could not unlock.\n");
    }
}

void child2_thread(void *arg){
    printf("Child2 thread: Started.\n");
    if (thread_lock(1) != 0){
        printf("ERROR: Child 2 Could not acquire lock.\n");
    }
    printf("Child2 thread running.\n");
    if (thread_unlock(1) != 0){
        printf("ERROR: Child 2 Could not unlock.\n");
    }
}

void child3_thread(void *arg){
    printf("Child3 thread: Started.\n");
    if (thread_lock(1) != 0){
        printf("ERROR: Child 3 Could not acquire lock.\n");
    }
    printf("Child3 thread running.\n");
    if (thread_unlock(1) != 0){
        printf("ERROR: Child 3 Could not unlock.\n");
    }
}

void child4_thread(void *arg){
    printf("Child4 thread: Started.\n");
    if (thread_lock(1) != 0){
        printf("ERROR: Child 4 Could not acquire lock.\n");
    }
    printf("Child4 thread running.\n");
    if (thread_unlock(1) != 0){
        printf("ERROR: Child 4 Could not unlock.\n");
    }
}

void child5_thread(void *arg){
    printf("Child5 thread: Started.\n");
    if (thread_lock(1) != 0){
        printf("ERROR: Child 5 Could not acquire lock.\n");
    }
    printf("Child5 thread running.\n");
    if (thread_unlock(1) != 0){
        printf("ERROR: Child 5 Could not unlock.\n");
    }
}