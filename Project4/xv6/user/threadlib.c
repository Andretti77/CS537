#include "types.h"
#include "stat.h"
#include "user.h"
#include "x86.h"

int
thread_create(void (*start_routine)(void*), void *arg)
{
	
	void *stack;
	stack = (void *)malloc(4096);
	return clone(start_routine, arg, stack);
	
}

int
thread_join(void)
{
	
	void **stack;	
	stack = NULL;
	int pid;
	pid = join(stack);
	
	if(pid == -1)
		return -1;
	
	free(*stack);
	return pid;
}

void 
lock_init(lock_t *lock)
{
	*lock = 0;
}

void
lock_acquire(lock_t *lock)
{
    while(xchg(lock,1) != 0);
}

void
lock_release(lock_t *lock)
{
    xchg(lock, 0);
}

void 
cond_wait(cond_t *cond, lock_t *lock)
{
	cond_wait_k(cond, lock);
}

void
cond_signal(cond_t *cond)
{
	cond_signal_k(cond);
}

void
cond_init(cond_t *cond)
{
	cond_init_k(cond);
}

