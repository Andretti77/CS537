#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <signal.h>
#include <string.h>

#define SHMNAME "petti_klisch"
#define PAGESIZE 4096
#define MAXCLIENTS 64

// Mutex variables
pthread_mutex_t* mutex;
pthread_mutexattr_t mutexAttribute;

//pointer for the shared memory
void* ptr;

typedef struct {
	char valid;
	int pid;
	char birth[25];
	char clientString[10];
	int elapsed_sec;
	double elapsed_msec;
} stats_t;

void exit_handler(int sig)
{
	if(shm_unlink(SHMNAME) != 0)
		exit(1);	

	if(munmap(ptr, PAGESIZE) != 0)
		exit(1);

	exit(0);
}

int main(int argc, char *argv[])
{
	if(argc > 1)
		exit(1);
  
	//setting up sighandler
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_handler = exit_handler;
	act.sa_flags = 0;

	if(sigaction(SIGTERM, &act, NULL) != 0)
		exit(1);

	if(sigaction(SIGINT, &act, NULL) != 0)
		exit(1);
	
	// Creating a new shared memory segment
	int fd_shm = shm_open(SHMNAME,  O_CREAT|O_RDWR, 0660);
	if(fd_shm == -1)
		exit(1);
  
	//truncating the shared memory to one page size
	int t = ftruncate(fd_shm, PAGESIZE);
	if(t == -1)
		exit(1);
  
	//setting up pointer
	ptr = mmap(NULL, PAGESIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd_shm, 0);
	if(ptr == NULL)
		exit(1);

  //lock is the first entry in the shared memory
	mutex = (pthread_mutex_t*) ptr;

	// Initializing mutex
	pthread_mutexattr_init(&mutexAttribute);
	pthread_mutexattr_setpshared(&mutexAttribute, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(mutex, &mutexAttribute);

  //iteration counter for printing
	int it = 1;

	while(1){
		sleep(1);
    //printing out information of all the clients
		for(int i = 1; i < MAXCLIENTS; i++){
			stats_t* currClient = (ptr+MAXCLIENTS*i);
			if(currClient->valid != 0){
				printf("%d, pid : %d, ", it, currClient->pid);
				printf("birth : %s, ", currClient->birth);
				printf("elapsed : %d s %.4f ms, %s\n", currClient->elapsed_sec, 
					currClient->elapsed_msec, currClient->clientString);				
			}
		}
		it++;
	}

	return 0;
}
