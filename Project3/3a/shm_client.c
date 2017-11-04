#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <time.h>
#include <sys/time.h>
#include <string.h>
#include <signal.h>
// ADD NECESSARY HEADERS
#define SHMNAME "petti_klisch"
#define PAGESIZE 4096
#define MAXCLIENTS 64
// Mutex variables
pthread_mutex_t* mutex;


typedef struct{
    char valid;
    int pid;
    char birth[25];
    char clientString[10];
    int elapsed_sec;
    double elapsed_msec;
} stats_t;

void* ptr;
stats_t* thisClient;

void exit_handler(int sig) {
    // ADD

    // critical section begins
	pthread_mutex_lock(mutex);

    // Client leaving; needs to reset its segment   
    thisClient->valid = 0;
	pthread_mutex_unlock(mutex);
	// critical section ends

    exit(0);
}

int main(int argc, char *argv[]) {
	// ADD    
    
    if(argc!=2)
        exit(1);

    struct sigaction act;
    memset(&act, 0 , sizeof(act));
    act.sa_handler = exit_handler;
    act.sa_flags = 0;

    if(sigaction(SIGINT, &act, NULL) < 0)
        return 1;

    if(sigaction(SIGTERM, &act, NULL) <0)
        return 1;
    

    int fd_shm = shm_open(SHMNAME, O_RDWR, 0660);
    if(fd_shm == -1)
        exit(1);

    ptr = mmap(NULL , 4096, PROT_WRITE | PROT_READ, MAP_SHARED, fd_shm, 0);
    if(ptr == (void*) -1)
        exit(1);

    mutex = (pthread_mutex_t*) ptr;

    time_t tic;
    struct tm* now;
    char created[25];
    time(&tic);    
    now = localtime(&tic);
    strftime(created, 25, "%c", now);

    struct timeval time_start, time_curr;
    gettimeofday(&time_start, NULL);

    // critical section begins
    pthread_mutex_lock(mutex);
        
	// client updates available segment
    for(int i =1; i<MAXCLIENTS; i++){

        thisClient = (stats_t*) (ptr+64*i);
        if(thisClient -> valid == 0){
            thisClient -> pid = getpid();
            strcpy(thisClient->birth, created);
            strcpy(thisClient->clientString, argv[1]);
            thisClient->elapsed_sec = 0;
            thisClient->elapsed_msec = 0;
            thisClient->valid = 1;
            break;
        }
        
        if(i == 63)
            exit(1);
    } 
		
	pthread_mutex_unlock(mutex);
    // critical section ends
        
	while (1) {
        
		// ADD
        gettimeofday(&time_curr, NULL);
  //      pthread_mutex_lock(mutex);

        thisClient->elapsed_sec = time_curr.tv_sec - time_start.tv_sec;
        thisClient->elapsed_msec = (time_curr.tv_usec - time_start.tv_usec)/1000.0;
        
//        pthread_mutex_unlock(mutex);

        sleep(1);

		// Print active clients
        printf("Active clients: ");
        for(int i = 1; i<MAXCLIENTS; i++){

            stats_t* temp = (stats_t*) (ptr+64*i);
            if(temp->valid == 1)
                printf("%d", temp->pid);

        }
        printf("\n");
    }

    return 0;
}
