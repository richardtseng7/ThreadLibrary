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
    thread_libinit((thread_startfunc_t) parent_thread, &x);
    
}
void parent_thread(void *arg){
    cout << "Parent thread: Started.\n";
    cout << "arg = " << * (int *) arg << endl;
    if (thread_lock(1) != 0){
        printf("ERROR: Thread could not acquire lock. \n");
    }
    if (thread_lock(2) != 0){
        printf("ERROR: Thread could not acquire second lock. \n");
    }
    thread_create((thread_startfunc_t) child0_thread, arg);
    if(thread_wait(2, 3) != -1){
        printf("ERROR: Thread never acquired the lock to wait on.\n");
    }
    else{
        printf("Correct error handling for waiting on lock that was never acquired.\n");
    }
    if (thread_wait(1, 4) != 0){
        printf("ERROR: Thread could not wait.\n");
    }
    if (thread_unlock(1) != 0){
        printf("ERROR: Thread could not unlock held lock successfully.\n");
    }
    if (thread_unlock(2) != 0){
        printf("ERROR: Thread could not unlock second held lock successfully. \n");
    }
    cout << "Parent thread: Finished.\n";
}
void child0_thread(void *arg){
    printf("Child0 thread: Started.\n");
    thread_lock(1);
    thread_signal(1, 4);
    thread_unlock(1);
    printf("Child0 thread: Finished.\n");
}