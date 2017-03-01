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
    if (thread_signal(1, 3) != 0){
        printf("ERROR: signal failed.\n");
    }
    if (thread_broadcast(21, 7) != 0){
        printf("ERROR: signal failed.\n");
    }    
    thread_lock(1);
    thread_create((thread_startfunc_t) child0_thread, arg);
    if (thread_yield() == -1){
        printf("ERROR: Yield failed.\n");
    }
    cout << "Parent thread: Finished.\n";
}

void child0_thread(void *arg){
    printf("Child0 thread: Started.\n");
    thread_lock(2);
    thread_yield();
    for (int i = 3; i < 10; i++){
        thread_lock(i);
        printf("Lock %d\n", i);
    }
    if (thread_yield() == -1){
        printf("ERROR: Yield failed.\n");
    }
    if (thread_signal(1, 3) == -1){
        printf("ERROR: Signal failed.\n");
    }
    if (thread_broadcast(4, 3) == -1){
        printf("ERROR: Broadcast failed.\n");
    }
    if (thread_create((thread_startfunc_t) parent_thread, arg) == -1){
        printf("ERROR: Thread could not be created.\n");
    }
    if (thread_create((thread_startfunc_t) child0_thread, arg) == -1){
        printf("ERROR: Thread could not be created.\n");
    }
    if (thread_yield() == -1){
        printf("ERROR: Yield failed.\n");
    }
    for (int i = 10; i > 3; i--){
        thread_unlock(i);
        printf("Lock %d unlocked\n", i);
    }
    thread_unlock(2);
    if (thread_yield() == -1){
        printf("ERROR: Yield failed.\n");
    }
    printf("Child0 thread: Finished.\n");
}