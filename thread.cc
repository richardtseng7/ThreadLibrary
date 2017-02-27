#include <ucontext.h>
#include "interrupt.h"
#include "thread.h"
#include <queue>
#include <map>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <new>
using namespace std;

struct Thread{
	unsigned int threadID;
	ucontext_t *context;
	char *stack;
	bool done;
	};

inline const char * const BoolToString(bool b)
{
  return b ? "true" : "false";
}

static void printQ(std::queue<Thread> aQ){
	int size = aQ.size();
	printf("Queue: ");

	for (int i=0;i<size;i++) {
		Thread cur = aQ.front();
		aQ.pop();
		aQ.push(cur);
		printf("%d, ", cur.threadID);
	}

	printf("\n");
}


//declare queues of u context here
static std::queue<Thread> readyQueue;
static std::map<unsigned int, std::queue<Thread> > lockQueue;
static std::map<unsigned int, unsigned int> lockTaken; //
static std::map<unsigned int, std::queue<Thread> > CVQueue;

static bool initialized = false;
static bool disabled;
static Thread current;
static ucontext_t *iter;

static int thread_count = 1;

static int run(thread_startfunc_t func, void *arg){
	if (!disabled){
		disabled = true;
		interrupt_disable();
	}	
	func(arg);
	if (disabled){
		disabled = false;
		interrupt_enable();
	}
	//printf("RUN: lockq size: %d, lockTaken %s\n", (int) lockQueue[1].size(), BoolToString(lockTaken[1]));
	current.done = true;
	swapcontext(current.context, iter);
	
	return 0;
}

int thread_libinit(thread_startfunc_t func, void *arg){
	//handling error: check if init is true, then return -1
	if (initialized){
		return -1; //error
	}
	initialized = true;
	//handling error: this thread is not created, return -1
	if(thread_create(func,arg) < 0){
		return -1;
	}
	if (!disabled){
		disabled = true;
		interrupt_disable();
	}
	Thread front = readyQueue.front();
	readyQueue.pop();
	current = front;
	//set up iterator as current context
	try {
		iter = new ucontext_t;
	} 
	catch (std::bad_alloc& ba){
		return -1;
	}
	
	getcontext(iter);
	//run front thread (master)
	swapcontext(iter, front.context);
	//comes back here after swap in runFunc
	while (readyQueue.size() > 0){		
			if (current.done){
				delete current.context;
				delete current.stack;
			}
			Thread next = readyQueue.front();
			readyQueue.pop();		
			current = next;
			swapcontext(iter, current.context);
	}
	delete iter;
	if (disabled){
		disabled = false;
		interrupt_enable();
	}
	cout << "Thread library exiting.\n";
	exit(0);
	return 0;
}

int thread_create(thread_startfunc_t func, void *arg){
	// initialize thread context
	if (!initialized){
		return -1;
	}
	if (!disabled){
		disabled = true;
		interrupt_disable();
	}
	Thread t;
	try {
		t.context = new ucontext_t;
		t.stack = new char [STACK_SIZE];
		t.context->uc_stack.ss_sp = t.stack;
		t.context->uc_stack.ss_size = STACK_SIZE;
		t.context->uc_stack.ss_flags = 0;
		t.context->uc_link = NULL;
		t.done = false;
	}
	catch (std::bad_alloc& ba){
		return -1;
	}

	t.threadID = thread_count;
	thread_count++;

	getcontext(t.context);
	makecontext(t.context, (void (*)()) run, 2, func, arg);
	//add this thread to ready queue
	readyQueue.push(t);
	if (disabled){
		disabled = false;
		interrupt_enable();
	}
	return 0;
}

// YIELD gives up control of CPU, not control of the lock.
int thread_yield(void){
	if (!initialized){
		return -1;
	}
	if (!disabled){
		disabled = true;
		interrupt_disable();
	}
	getcontext(current.context); // store current thread context in t
	readyQueue.push(current); // thread moved to the back of the R queue
	swapcontext(current.context, iter);
	if (disabled){
		disabled = false;
		interrupt_enable();
	}
	return 0;
}

int thread_lock(unsigned int lock){
	// thread (usually first one in R queue) acquires lock,
	if (!initialized){
		return -1;
	}
	if (!disabled){
		disabled = true;
		interrupt_disable();
	}
	//first time calling
	if (lockTaken[lock] == 0) {
		lockTaken[lock] = current.threadID;
	} 
	//if lock is taken, put the calling thread into the lock queue 
	else{
		// handling error: check if thread already has the lock
		if (lockTaken[lock] == current.threadID) {
			return -1;
		}
		lockQueue[lock].push(current);
		swapcontext(current.context, iter);
	}
	//if its not taken, set locktaken[this id] = false 
	if (disabled){
		disabled = false;
		interrupt_enable();
	}
	return 0;
}

int thread_unlock(unsigned int lock){
	// thread frees lock
	if (!initialized){
		return -1;
	}
	// handling error: tries to unlock a lock it doesn't have
	if (lockTaken[lock] != current.threadID) {
		return -1;
	}
	if (!disabled){
		disabled = true;
		interrupt_disable();
	}
	lockTaken[lock] = 0;
	if (lockQueue[lock].size() > 0){
		readyQueue.push(lockQueue[lock].front());
		lockTaken[lock] = lockQueue[lock].front().threadID;
		lockQueue[lock].pop();
	}
	if (disabled){
		disabled = false;
		interrupt_enable();
	}
	return 0;
}

// WAIT: 
// Caller unlocks the lock, blocks (running --> blocked), and moves to tail of the CVQueue. 
// Another thread from the front of the R queue runs (ready --> running)
// Later, when the caller wakes up (e.g., due to thread_signal) and runs, it re-acquires the lock and returns.

int thread_wait(unsigned int lock, unsigned int cond){
	// gives up lock
	if (!initialized){
		return -1;
	}
	// printf("BEFORE WAIT:");
	// printQ(readyQueue);
	// printQ(CVQueue[cond]);
	if (!disabled){
		disabled = true;
		interrupt_disable();
	}
	thread_unlock(lock);
	CVQueue[cond].push(current);
	// printf("AFTER WAIT:");
	// printQ(readyQueue);
	// printQ(CVQueue[cond]);
	swapcontext(current.context, iter);
	thread_lock(lock);
	// printf("AFTER WAKEUP FROM SIGNAL:");
	// printQ(readyQueue);
	// printQ(CVQueue[cond]);
	if (disabled){
		disabled = false;
		interrupt_enable();
	}
	return 0;
}

int thread_signal(unsigned int lock, unsigned int cond){
	if (!initialized){
		return -1;
	}
	// printf("BEFORE SIGNAL:");
	// printQ(readyQueue);
	// printQ(CVQueue[cond]);
	if (!disabled){
		disabled = true;
		interrupt_disable();
	}
	readyQueue.push(CVQueue[cond].front());
	CVQueue[cond].pop();
	if (disabled){
		disabled = false;
		interrupt_enable();
	}	// printf("AFTER SIGNAL:");
	// printQ(readyQueue);
	// printQ(CVQueue[cond]);
	return 0;
}

int thread_broadcast(unsigned int lock, unsigned int cond){
	if (!initialized){
		return -1;
	}
	if (!disabled){
		disabled = true;
		interrupt_disable();
	}	
	while(CVQueue[cond].size() > 0){
		readyQueue.push(CVQueue[cond].front());
		CVQueue[cond].pop();
	}
	if (disabled){
		disabled = false;
		interrupt_enable();
	}
	return 0;
}	

