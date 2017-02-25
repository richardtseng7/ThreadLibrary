#include <ucontext.h>
#include "interrupt.h" 
#include <queue>

//declare queues of u context here
std::queue<ucontext_t *> readyQueue;
std::queue<ucontext_t *> LockQueue;

//initialize ucp here

int thread_libinit(thread_startfunc_t func, void *arg) {
	//initialize context
	ucontext_t t;
	char *stack = new char [STACK_SIZE];
	t->uc_stack.ss_sp = stack;
	t->uc_stack.ss_size = STACK_SIZE;
	t->uc_stack.ss_flags = 0;
	t->uc_link = NULL;

	getcontext(t);
	makecontext(t,func,arg);

	//add this thread to ready queue
	readyQueue.push(t);

	//when done delete this thread from the queue
}

int thread_create(thread_startfunc_t func, void *arg) {
	//initialize context
	ucontext_t t;
	char *stack = new char [STACK_SIZE];
	t->uc_stack.ss_sp = stack;
	t->uc_stack.ss_size = STACK_SIZE;
	t->uc_stack.ss_flags = 0;
	t->uc_link = NULL;

	getcontext(t);
	makecontext(t,func,arg);

	//add this thread to ready queue
	readyQueue.push(t);

}

int thread_yield(void) {
	//if queue is empty, exit
	if (readyQueue.isEmpty()) {
		cout << "Thread library exiting.\n";
		exit(0);
	}
	
	//store ucp to current thread context
	//add current thread context to queue
	//get the context of the first one in the queue to ucp

}

int thread_lock(unsigned int lock) {

}

int thread_unlock(unsigned int lock) {

}

int thread_wait(unsigned int lock, unsigned int cond) {

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