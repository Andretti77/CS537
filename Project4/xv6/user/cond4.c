/* race condition in CV? (must atomically release lock and sleep) */
#include "types.h"
#include "user.h"

#undef NULL
#define NULL ((void*)0)

#define PGSIZE (4096)

#define N 1
int ppid;
int pid1, pid2;
volatile int bufsize;
int result;
lock_t lock;
cond_t nonfull, nonempty;

#define assert(x) if (x) {} else { \
   printf(1, "%s: %d ", __FILE__, __LINE__); \
   printf(1, "assert failed (%s)\n", # x); \
   printf(1, "TEST FAILED\n"); \
   kill(ppid); \
   exit(); \
}

void produce(void *arg);
void consume(void *arg);

int
main(int argc, char *argv[])
{
  int i;
  ppid = getpid();

  lock_init(&lock);
  cond_init(&nonfull);
  cond_init(&nonempty);
  bufsize = 0;

  assert((pid1 = thread_create(produce, NULL)) > 0);
  assert((pid2 = thread_create(consume, NULL)) > 0);
  
  for(i = 0; i < 500; i++) {
    result <<= 1;
    sleep(1);
  }

  if(result & 0x3ff)
    printf(1, "TEST PASSED\n");
  
  exit();
}

void
produce(void *arg) {
  while(1) {
		printf(1,"producer acquire lock\n");
    lock_acquire(&lock);
		printf(1,"producer acquire lock finished\n");
    while(bufsize == N){
			printf(1,"producer wait\n");
      cond_wait(&nonfull, &lock);
			printf(1,"producer wait finished\n");
		}
    result <<= 1;
    result |= 1;

    bufsize++;
    cond_signal(&nonempty);
		printf(1,"producer release lock\n");
    lock_release(&lock);
  }
}

void
consume(void *arg) {
  while(1) {
		printf(1,"consumer acquire lock\n");
    lock_acquire(&lock);
		printf(1,"consumer acquire lock finished\n");
    while(bufsize == 0){
			printf(1,"consumer wait\n");
      cond_wait(&nonempty, &lock);
			printf(1,"consumer wait finished\n");
		}
    result <<= 1;
    result |= 1;

    bufsize--;
    cond_signal(&nonfull);
		printf(1,"consumer release lock\n");
    lock_release(&lock);
  }
}
