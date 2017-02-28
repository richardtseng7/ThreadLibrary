#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "thread.h"
using namespace std;

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
    thread_wait(1, 3);
    thread_unlock(1);
    cout << "Parent thread: Finished.\n";
}

void child0_thread(void *arg){
    printf("Child0 thread: Started.\n");
    thread_lock(2);
    if (thread_wait(2, 3) != -1){
        printf("ERROR: One CV must be bound to only one lock\n");
    }
    thread_signal(1, 3);
    thread_unlock(2);
    printf("Child0 thread: Finished.\n");
}