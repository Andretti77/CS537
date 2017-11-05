#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
#include <time.h>
#include <signal.h>
#include <string.h>
#include <sys/time.h>

typedef struct {
	int pid;
	char birth[25];
	char clientString[10];
	int elapsed_sec;
	double elapsed_msec;
} stats_t;

// Mutex variables
pthread_mutex_t* mutex;
//pointer to the shared memory
void * ptr;
//this processes client
stats_t* thisClient;

void exit_handler(int sig) {

	// critical section begins
	pthread_mutex_lock(mutex);

	// Client leaving; needs to reset its segment
	thisClient->pid = -1;

	pthread_mutex_unlock(mutex);
	// critical section ends

	exit(0);
}

int main(int argc, char *argv[]) {

	if (argc != 2)
		exit(1);


    //setting up sighandler
	struct sigaction act;
	memset(&act, 0, sizeof(act));
	act.sa_handler = exit_handler;
	act.sa_flags = 0;
	if(sigaction(SIGINT, &act, NULL) != 0)
		exit(1);
	if(sigaction(SIGTERM, &act, NULL) != 0)
		exit(1);

	
    //file descriptor for the shared memory
	int fd_shm = shm_open("petti_klisch", O_RDWR, 0660);
	if (fd_shm == -1)
		exit(1);

    //setting up the pointer
	ptr = mmap(NULL, 4096, PROT_WRITE | PROT_READ, MAP_SHARED, fd_shm, 0);
	if (ptr == (void *)-1)
		exit(1);

    //lock is in the first entry in shared memory
	mutex = (pthread_mutex_t*) ptr;


    //setting up time variables for birth
	time_t t;
	struct tm *now;
	char time_buffer[25];
	time (&t);
	now = localtime(&t);
	strftime(time_buffer, 25, "%c", now);

    //time variables for elapsed time 
	struct timeval start_time;
	struct timeval curr_time;
	gettimeofday(&start_time, NULL);
    int i;
	// critical section begins
	pthread_mutex_lock(mutex);

    //finding the first slot available in the shared memory
    //and adding the client to this
    //first slot is indicated by a pid of -1
    //
    //starting at 1 to skip over the lock
	for (i = 1; i < 64; i++){
		thisClient = (stats_t *) (ptr + 64 * i);
		if (thisClient->pid == -1){
            //found the first available slot
			thisClient->pid = getpid();
			strcpy(thisClient->birth, time_buffer);
			strcpy(thisClient->clientString, argv[1]);
			thisClient->elapsed_sec = 0;
			thisClient->elapsed_msec = 0;
            //do not want to continue in loop so break
			break;
		}

	}
	// client updates available segment

	pthread_mutex_unlock(mutex);
	// critical section ends

    //max clients has been reached so no more clients allowed so exit
    if(i > 63)
        exit(1);


	while (1) {
        //updating elapesed time for this process
        gettimeofday(&curr_time, NULL);
		thisClient->elapsed_sec = (curr_time.tv_sec - start_time.tv_sec);
		thisClient->elapsed_msec = (curr_time.tv_usec - start_time.tv_usec) / 100.0;
		sleep(1);

		// Print active clients
		printf("Active clients : ");
		// critical section begins

		pthread_mutex_lock(mutex);
		for (int i = 1; i < 64; i++){
			stats_t* currClient = (stats_t *) (ptr + 64 * i);
			if(currClient->pid != -1){
				printf("%d ", currClient->pid);
				
			}

		}
		printf("\n");
		pthread_mutex_unlock(mutex);
		// critical section ends
	}

	return 0;
}
