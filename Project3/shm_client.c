#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <time.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
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
    float elapsed_msec;
}stats_t;

stats_t* shm_ptr = NULL;
stats_t* currClient = NULL ; 

static void exit_handler(int sig) {
    // ADD

    // critical section begins
//	pthread_mutex_lock(mutex);

    // Client leaving; needs to reset its segment   
    currClient->valid = 0;
   // memcpy(shm_ptr+pos, currClient, 64);
//	pthread_mutex_unlock(mutex);
	// critical section ends

    exit(0);
}

int main(int argc, char *argv[]) {
	// ADD


    if(argc!=2)
        exit(1);
    struct sigaction act;
    memset(&act, '\0', sizeof(act));
    act.sa_handler = exit_handler;
    act.sa_flags = SA_SIGINFO;

    if(sigaction(SIGINT, &act, NULL)<0)
        return 1; 

    if(sigaction(SIGTERM, &act, NULL)<0)
        return 1;

	int fd = shm_open(SHMNAME, O_RDWR, 0660);
    if(fd == -1)
       exit(1);
    
    shm_ptr = (stats_t*)mmap(NULL, PAGESIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(shm_ptr == (void*) -1)
        exit(1);
    
    //time variables
    time_t tictoc;
    time(&tictoc);
    struct tm* now;
    struct timeval time_start, time_curr;
    char time[25];
    now = localtime(&tictoc);
    strftime(time, 25, "%c", now);
    gettimeofday(&time_start , NULL);    
    
    // critical section begins
    mutex = (pthread_mutex_t*)shm_ptr;
    

	// client updates available segment
//    pthread_mutex_lock(mutex);
    for(int i = 1; i<MAXCLIENTS; i++){
        currClient = ((void*)shm_ptr+64*i);
        if(currClient -> valid == 0){

            currClient -> valid = 1;
            currClient -> pid = getpid();
            strcpy(currClient -> birth, time);
            currClient -> elapsed_sec = 0;
            currClient -> elapsed_msec = 0;
            strcpy(currClient -> clientString ,argv[1]);
            //memcpy(shm_ptr+i, currClient, 64);
            break;
        }

       if(i == 63){
  //         pthread_mutex_unlock(mutex);
           exit(1);
       }
    }
    
   // pthread_mutex_unlock(mutex);
    // critical section ends

    while (1) {
		// ADD
		// Print active clients
        //
        
        gettimeofday(&time_curr, NULL);
        currClient->elapsed_sec = time_curr.tv_sec - time_start.tv_sec;
        currClient->elapsed_msec = (time_curr.tv_usec - time_start.tv_usec)/1000.0;
        
        sleep(1);
        printf("Active clients: ");
        for(int i = 1; i<MAXCLIENTS; i++){
        //    pthread_mutex_lock(mutex);
            stats_t* currClient_temp = ((void*)shm_ptr+i*64);
            if(currClient_temp->valid == 1){
                printf("%d ", currClient_temp->pid);
            }
          //  pthread_mutex_unlock(mutex);
        } 
        printf("\n");

    }

    return 0;
}
