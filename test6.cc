#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "thread.h"
using namespace std;

//1 cv is bound to exactly 1 mutex

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
    thread_lock(1);
    for(int i = 0; i < 10000; i++){
        if (thread_create((thread_startfunc_t) child0_thread, arg) == -1){
            return;
        }
    }
    printf("Thread create looped successfully.\n");
    thread_unlock(1);
    cout << "Parent thread: Finished.\n";
}
void child0_thread(void *arg){
    return;
}