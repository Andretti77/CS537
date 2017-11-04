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
    char valid;
    int pid;
    char birth[25];
    char clientString[10];
    int elapsed_sec;
    float elapsed_msec;
} stats_t;

stats_t* shm_ptr = NULL;

int numClients=0;
static void exit_handler(int sig) 
{
    // ADD
    if(munmap(shm_ptr, PAGESIZE)!=0)
        exit(1);
    if(shm_unlink(SHM_NAME)!=0)
        exit(1);

	exit(0);
}

int main(int argc, char *argv[]) 
{
    // ADD
	struct sigaction act;
    memset(&act, 0, sizeof(act));
    act.sa_handler = exit_handler;
    act.sa_flags = SA_SIGINFO;

    if(sigaction(SIGINT, &act, NULL)<0){
        return 1;
    }

    if(sigaction(SIGTERM, &act, NULL)<0)
        return 1;

    // Creating a new shared memory segment
	int fd = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0660);	
    if(fd == -1 )
        exit(1);

    int t =  ftruncate(fd, PAGESIZE);
    if(t == -1)
        exit(1);

    shm_ptr = (stats_t *) mmap(NULL, PAGESIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd ,0);
    if(shm_ptr == (void *) -1 )
        exit(1);

    mutex = (pthread_mutex_t*) shm_ptr;

    // Initializing mutex
	pthread_mutexattr_init(&mutexAttribute);
	pthread_mutexattr_setpshared(&mutexAttribute, PTHREAD_PROCESS_SHARED);
    pthread_mutex_init(mutex, &mutexAttribute);
   
    int i;
//    for(i = 1; i<MAXCLIENTS; i++){
  //      pthread_mutex_lock(mutex);
      //  stats_t* currClient = (stats_t*) (shm_ptr+64*i);
    //    currClient -> valid = 0;
       // pthread_mutex_unlock(mutex);
   // }

    int j = 1;

    while (1) 
	{
        
        sleep(1);
		// ADD
        
       //     pthread_mutex_lock(mutex);
        for(i = 1; i<MAXCLIENTS; i++){

            stats_t* currClient =  ((void*)shm_ptr+i*64);

            if(currClient->valid == 1){
                numClients++;
                printf("%d, pid : %d, birth : %s, elapsed : %d s %.4f ms, %s\n",
                        j, currClient->pid, currClient->birth, currClient->elapsed_sec, currClient->elapsed_msec, currClient->clientString);
            
            }
        }

         //   pthread_mutex_unlock(mutex);
        j++;
//        numClients = 0;
    }

    return 0;
}
