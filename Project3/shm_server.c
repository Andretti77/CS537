#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
// ADD NECESSARY HEADERS

// Mutex variables
pthread_mutex_t* mutex;
pthread_mutexattr_t mutexAttribute;

void exit_handler(int sig) 
{
    // ADD
    
	exit(0);
}

int main(int argc, char *argv[]) 
{
    // ADD
	
	// Creating a new shared memory segment
	int fd_shm = shm_open(SHM_NAME, O_RDWR | O_CREAT, 0660);	
        
    // Initializing mutex
	pthread_mutexattr_init(&mutexAttribute);
	pthread_mutexattr_setpshared(&mutexAttribute, PTHREAD_PROCESS_SHARED);
	pthread_mutex_init(mutex, &mutexAttribute);

    while (1) 
	{
		// ADD

        sleep(1);
    }

    return 0;
}
