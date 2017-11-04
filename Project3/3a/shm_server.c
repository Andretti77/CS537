#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <signal.h>
#include <string.h>
// ADD NECESSARY HEADERS
#define SHMNAME "petti_klisch"
#define PAGESIZE 4096
#define MAXCLIENTS 64
// Mutex variables
pthread_mutex_t* mutex;
pthread_mutexattr_t mutexAttribute;

void* ptr;


typedef struct{
    char valid;
    int pid;
    char birth[25];
    char clientString[10];
    int elapsed_sec;
    double elapsed_msec;
} stats_t;



void exit_handler(int sig) 
{
    // ADD
    if(munmap(ptr, PAGESIZE) != 0)
       exit(1);
    if(shm_unlink(SHMNAME) !=0)
       exit(1);


	exit(0);
}

int main(int argc, char *argv[]) 
{
    // ADD
	if(argc > 1)
        exit(1);

    struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = exit_handler;
    act.sa_flags = 0;

    if(sigaction(SIGINT, &act, NULL)<0)
        return 1;

    if(sigaction(SIGTERM, &act, NULL)<0)
        return 1;

	// Creating a new shared memory segment
    int fd_shm = shm_open(SHMNAME, O_RDWR | O_CREAT, 0660);	
    if(fd_shm ==-1)
        exit(1);

    int t = ftruncate(fd_shm, PAGESIZE);
    if(t == -1)
        exit(1);

    ptr = mmap(NULL, 4096, PROT_WRITE | PROT_READ, MAP_SHARED, fd_shm, 0);
    if(ptr == (void*) -1)
        exit(1);

    mutex = (pthread_mutex_t*)ptr;

    // Initializing mutex
	pthread_mutexattr_init(&mutexAttribute);
	pthread_mutexattr_setpshared(&mutexAttribute, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(mutex, &mutexAttribute);


    int j = 1;
    while (1) 
	{
		// ADD
        
        sleep(1);
        for(int i = 1; i<MAXCLIENTS; i++){
            stats_t* currClient = (stats_t*) (ptr+64*i);
            if(currClient -> valid == 1){
                printf("%d, pid: %d, ", j, currClient->pid);
                printf("birth: %s, ", currClient->birth);
                printf("elapsed: %d s %.4f ms, %s\n", currClient->elapsed_sec, currClient->elapsed_msec, currClient->clientString);
                j++;
            }
        }
    }

    return 0;
}
