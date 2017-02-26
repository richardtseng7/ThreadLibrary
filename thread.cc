#include <ucontext.h>
#include "interrupt.h"
#include "thread.h"
#include <queue>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

//declare queues of u context here
static std::queue<ucontext_t *> readyQueue;
static std::queue<ucontext_t *> LockQueue;

static ucontext_t *current;
static ucontext_t *scheduler;
static bool first = false;

//initialize ucp here

// int thread_libinit(thread_startfunc_t func, void *arg) {
// 	//handling error: check if init is true, then return -1
// 	if (first == true) {
// 		return -1; //error
// 	}
// 	first = true;
// 	thread_create(func, arg);
// 	current = new ucontext_t;
// 	current = readyQueue.front();
// 	readyQueue.pop();
// 	iter = new ucontext_t;
// 	getcontext(iter);
// 	swapcontext(iter, current);
// 	ucontext_t *next;
// 	while (!readyQueue.empty()){
// 		next = new ucontext_t;
// 		next = readyQueue.front();
// 		current = readyQueue.front();
// 		readyQueue.pop();
// 		swapcontext(iter, next);
// 	}
// 	return 0;
// }

int thread_libinit(thread_startfunc_t func, void *arg) {
	//handling error: check if init is true, then return -1
	if (first == true) {
		return -1; //error
	}
	first = true;
	thread_create(func, arg);
	current = new ucontext_t;
	current = readyQueue.front();
	readyQueue.pop();
	iter = new ucontext_t;
	getcontext(iter);
	swapcontext(iter, current);
	ucontext_t *next;
	while (!readyQueue.empty()){
		next = new ucontext_t;
		next = readyQueue.front();
		current = readyQueue.front();
		readyQueue.pop();
		swapcontext(iter, next);
	}
	return 0;
}

int thread_create(thread_startfunc_t func, void *arg) {
	// initialize thread context
	printf("thread_create is called!\n");
	
	ucontext_t *t = new ucontext_t;
	
	char *stack = new char [STACK_SIZE];
	t->uc_stack.ss_sp = stack;
	t->uc_stack.ss_size = STACK_SIZE;
	t->uc_stack.ss_flags = 0;
	t->uc_link = NULL;
	

	getcontext(t);
	makecontext(t, (void (*)()) func, 2, arg);
	//add this thread to ready queue
	readyQueue.push(t);


	return 0;
}

// YIELD gives up control of CPU, not control of the lock.
int thread_yield(void) {
	//if R queue is empty, exit
	if (readyQueue.empty()) {
		cout << "Thread library exiting.\n";
		exit(0);
	}
	
	//store ucp to current thread context
	//add current thread context to ready queue
	//get the context of the first thread in ready queue to ucp
	ucontext_t *t = new ucontext_t;
	getcontext(t); // store current thread context in t
	readyQueue.push(t); // thread moved to the back of the R queue

	ucontext_t *nt = new ucontext_t;
	nt = readyQueue.front();
	readyQueue.pop();
	swapcontext(t, nt);
	return 0;
}

int thread_lock(unsigned int lock) {
	// thread (usually first one in R queue) acquires lock,
}

int thread_unlock(unsigned int lock) {
	// thread frees lock
}

int thread_wait(unsigned int lock, unsigned int cond) {
	// gives up lock
}
int thread_signal(unsigned int lock, unsigned int cond) {

}
int thread_broadcast(unsigned int lock, unsigned int cond) {

}

// // cout << "Thread library exiting.\n";
// // exit(0);

// //describes the interface to the interrupt library that your thread library will use. 

// /*
// * Initialize a context structure by copying the current thread's context.
// */
// getcontext(ucontext_ptr); // ucontext_ptr has type (ucontext_t *)

// /*
// * Direct the new thread to use a different stack. Your thread library
// * should allocate STACK_SIZE bytes for each thread's stack.
// */
// char *stack = new char [STACK_SIZE];
// ucontext_ptr->uc_stack.ss_sp = stack;
// ucontext_ptr->uc_stack.ss_size = STACK_SIZE;
// ucontext_ptr->uc_stack.ss_flags = 0;
// ucontext_ptr->uc_link = NULL;

// /*
// * Direct the new thread to start by calling start(arg1, arg2).
// */
// makecontext(ucontext_ptr, (void (*)()) start, 2, arg1, arg2);

// // Use swapcontext to save the context of the current thread
// // and switch to the context of another thread. 
// // de-allocate the memory used for the thread's stack space & context when thread is done

// // DONE WITH ALL THREADS
