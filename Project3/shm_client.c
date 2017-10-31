#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <time.h>
#include <string.h>
// ADD NECESSARY HEADERS
#define SHMNAME "petti_klisch"
#define PAGESIZE 4096
#define MAXCLIENTS 64
// Mutex variables
pthread_mutex_t* mutex;


typedef struct{
    int pid;
    char birth[25];
    char clientString[10];
    int elapsed_sec;
    double elapsed_msec;
}stats_t;

void exit_handler(int sig) {
    // ADD

    // critical section begins
	pthread_mutex_lock(mutex);

    // Client leaving; needs to reset its segment   
    
	pthread_mutex_unlock(mutex);
	// critical section ends

    exit(0);
}

int main(int argc, char *argv[]) {
	// ADD    
	int fd = shm_open(SHMNAME, O_RDWR, 0660);
    if(fd == -1)
       exit(1); 
    void*  shm_ptr = mmap(NULL, PAGESIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
    if(shm_ptr == NULL)
        exit(1);

    //time variables
    time_t tictoc;
    time(&tictoc);
    struct tm* now;
    char time[25];
    now = localtime(&tictoc);

    strftime(time, 25, "%c", now);
    // critical section begins
    mutex = (pthread_mutex_t*)shm_ptr;
    pthread_mutex_lock(mutex);
    stats_t* currClient = NULL; 

	// client updates available segment
    for(int i = 1; i<MAXCLIENTS; i++){
        currClient = (stats_t *)shm_ptr+(i*64);
        if(currClient -> pid == -1)
            break;
    }
	currClient -> pid = getpid();
    strcpy(currClient -> birth, time);
    currClient -> elapsed_sec = 1;
    currClient -> elapsed_msec = 0;
    strcpy(currClient -> clientString ,argv[1]);
	pthread_mutex_unlock(mutex);
    // critical section ends
        
	while (1) {
        
		// ADD
		// Print active clients
        printf("Active clients: ");
        for(int i = 1; i<MAXCLIENTS; i++){
            pthread_mutex_lock(mutex);
            currClient = (stats_t*) shm_ptr+(i*64);
            if(currClient->pid != -1)
                printf("%d", currClient->pid);
            pthread_mutex_unlock(mutex);
        } 
        printf("\n");
        sleep(1);

    }

    return 0;
}
