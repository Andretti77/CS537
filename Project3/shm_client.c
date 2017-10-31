#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/mman.h>
// ADD NECESSARY HEADERS

// Mutex variables
pthread_mutex_t* mutex;

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
	
    // critical section begins
    pthread_mutex_lock(mutex);
        
	// client updates available segment
		
	pthread_mutex_unlock(mutex);
    // critical section ends
        
	while (1) {
        
		// ADD
        
        sleep(1);

		// Print active clients
    }

    return 0;
}
