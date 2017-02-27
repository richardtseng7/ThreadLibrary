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

static bool initialized = false;
static int thread_count = 1;
static Thread current;
static ucontext_t *iter;
static std::queue<Thread> readyQueue;
static std::map<unsigned int, std::queue<Thread> > lockQueue;
static std::map<unsigned int, unsigned int> lockTaken;
static std::map<unsigned int, std::queue<Thread> > CVQueue;


inline const char * const BoolToString(bool b)
{
    return b ? "true" : "false";
}


static void printQ(std::queue<Thread> aQ){
    int size = aQ.size();
    for (int i=0;i<size;i++) {
        Thread cur = aQ.front();
        aQ.pop();
        aQ.push(cur);
        printf("%d, ", cur.threadID);
    }
    printf("\n");
}


static int run(thread_startfunc_t func, void *arg){
    interrupt_enable();
	func(arg);
    interrupt_disable();
	current.done = true;
	swapcontext(current.context, iter);
	return 0;
}


int thread_libinit(thread_startfunc_t func, void *arg){
	//handling error: check if init is true, then return -1
	if (initialized){
		return -1;
	}
	initialized = true;
	//handling error: this thread is not created, return -1
	if(thread_create(func,arg) < 0){
		return -1;
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
    interrupt_disable();
	swapcontext(iter, front.context);
	//comes back here after swap in run
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
	cout << "Thread library exiting.\n";
	exit(0);
	return 0;
}


int thread_create(thread_startfunc_t func, void *arg){
	// initialize thread context
	if (!initialized){
		return -1;
	}
    interrupt_disable();
	Thread t;
	try {
		t.context = new ucontext_t;
		t.stack = new char [STACK_SIZE];
        getcontext(t.context);
		t.context->uc_stack.ss_sp = t.stack;
		t.context->uc_stack.ss_size = STACK_SIZE;
		t.context->uc_stack.ss_flags = 0;
		t.context->uc_link = NULL;
		t.done = false;
        t.threadID = thread_count;
        thread_count++;
        makecontext(t.context, (void (*)()) run, 2, func, arg);
        //add this thread to ready queue
        readyQueue.push(t);
	}
	catch (std::bad_alloc& ba){
        interrupt_enable();
        return -1;
	}
    interrupt_enable();
	return 0;
}


int thread_yield(void){
	if (!initialized){
		return -1;
	}
    interrupt_disable();
	getcontext(current.context); // store current thread context in t
	readyQueue.push(current); // thread moved to the back of the R queue
	swapcontext(current.context, iter);
    interrupt_enable();
	return 0;
}


static int toLock(unsigned int lock){
    //first time calling
    if (lockTaken[lock] == 0){
        lockTaken[lock] = current.threadID;
    }
    //if lock is taken, put the calling thread into the lock queue
    else {
        // handling error: check if thread already has the lock
        if (lockTaken[lock] == current.threadID) {
            return -1;
        }
        lockQueue[lock].push(current);
        swapcontext(current.context, iter);
    }
    return 0;
}


int thread_lock(unsigned int lock){
	//thread (usually first one in R queue) acquires lock,
	if (!initialized){
		return -1;
	}
    interrupt_disable();
    if(toLock(lock)==-1) {
    	interrupt_enable();
    	return -1;
    }
    //if its not taken, set locktaken[this id] = false
    interrupt_enable();
	return 0;
}


static int toUnlock(unsigned int lock){
	if (lockTaken.count(lock)==0) {
		return -1;
	}
    if (lockTaken[lock] != current.threadID){
        return -1;
    }
    lockTaken[lock] = 0;
    if (lockQueue[lock].size() > 0){
        readyQueue.push(lockQueue[lock].front());
        lockQueue[lock].pop();
        lockTaken[lock] = lockQueue[lock].front().threadID;
    }
    return 0;  
}
int thread_unlock(unsigned int lock){
	// thread frees lock
	if (!initialized){
		return -1;
	}

    interrupt_disable();
    if (toUnlock(lock)==-1) {
    	interrupt_enable();
    	return -1;
    }
    interrupt_enable();
	return 0;
}


int thread_wait(unsigned int lock, unsigned int cond){
	if (!initialized){
		return -1;
	}
    interrupt_disable();
	if (toUnlock(lock)==-1) {
		interrupt_enable();
		return -1;
	}
    CVQueue[cond].push(current);
    ucontext_t *t = new ucontext_t;
    t = current.context;
	swapcontext(current.context, iter);
	if (toLock(lock)==-1) {
		interrupt_enable();
		return -1;
	}
 	interrupt_enable();
	return 0;
}


int thread_signal(unsigned int lock, unsigned int cond){
	if (!initialized){
		return -1;
	}
    interrupt_disable();
    if (CVQueue[cond].size() > 0){
        Thread t = CVQueue[cond].front();
        CVQueue[cond].pop();
        readyQueue.push(t);
    }
    interrupt_enable();
	return 0;
}


int thread_broadcast(unsigned int lock, unsigned int cond){
	if (!initialized){
		return -1;
	}
    interrupt_disable();
	while(CVQueue[cond].size() > 0){
		readyQueue.push(CVQueue[cond].front());
		CVQueue[cond].pop();
	}
    interrupt_enable();
	return 0;
}