#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <string.h>
#include <signal.h>
// ADD NECESSARY HEADERS
#define SHM_NAME "petti_klisch"
#define PAGESIZE 4096
#define MAXCLIENTS 64
// Mutex variables
pthread_mutex_t* mutex;
pthread_mutexattr_t mutexAttribute;

typedef struct { 
    int pid;
    char birth[25];
    char clientString[10];
    int elapsed_sec;
    double elapsed_msec;
} stats_t;


int numClients=0;
void exit_handler(int sig) 
{
    // ADD
    
	exit(0);
}

int main(int argc, char *argv[]) 
{
    // ADD
	struct sigaction act;
    memset(&act, '\0', sizeof(act));
    act.sa_sigaction = &exit_handler;
    act.sa_flags = SA_SIGINFO;

    if(sigaction(SIGINT | SIGTERM, &act, NULL)<0){
        return 1;
    }
	// Creating a new shared memory segment
	int fd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0660);	
    if(fd == -1 )
        exit(1);
    ftruncate(fd, PAGESIZE);
    void * shm_ptr =  mmap(NULL, PAGESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd ,0);
    if(shm_ptr == NULL)
        exit(1);

    mutex = (pthread_mutex_t*) shm_ptr;

    // Initializing mutex
	pthread_mutexattr_init(&mutexAttribute);
	pthread_mutexattr_setpshared(&mutexAttribute, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(mutex, &mutexAttribute);
   
    int i;
    //setting all pids to -1 to see where to add new client data
    for(i =1; i<MAXCLIENTS; i++){
        stats_t* currClient = (stats_t*) shm_ptr+(i*64);    
        currClient->pid = -1;

    }

    while (1) 
	{
		// ADD
        for(i = 1; i<MAXCLIENTS; i++){

            pthread_mutex_lock(mutex);
            stats_t* currClient = (stats_t*)shm_ptr+(i*64);

            if(currClient->pid != -1)
                printf("%d, pid : %d, birth : %s, elapsed : %d s %f ms, %s\n",
                        i, currClient->pid, currClient->birth, currClient->elapsed_sec, currClient->elapsed_msec, currClient->clientString);

            pthread_mutex_unlock(mutex);
        }
        
        sleep(1);
    }

    return 0;
}
