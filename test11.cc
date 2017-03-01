#include <stdio.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <iostream>
#include "thread.h"
using namespace std;

void start(void *arg);
void one(void *arg);
void two(void *arg);
void three(void *arg);

int main(int argc, char *argv[]){
    int x = 7;
    thread_libinit((thread_startfunc_t) start, &x);
}

void one(void *arg){
    thread_lock(0);
    printf("one wait\n");
    thread_wait(0, 0);
    printf("one awake\n");
    thread_unlock(0);
    printf("one done\n");
}

void two(void *arg){
    thread_lock(0);
    printf("two yield\n");
    thread_yield();
    printf("two returns\n");
    thread_unlock(0);
    printf("two done\n");
}

void three(void *arg){
    thread_signal(0, 0);
    printf("three signals\n");
    thread_lock(0);
    printf("three gets lock\n");
    thread_unlock(0);
    printf("three done\n");
}

void start(void *arg){
    thread_create((thread_startfunc_t) one, arg);
    thread_create((thread_startfunc_t) two, arg);
    thread_create((thread_startfunc_t) three, arg);
}