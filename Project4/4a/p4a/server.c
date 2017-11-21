#include "cs537.h"
#include "request.h"

// 
// server.c: A very, very simple web server
//
// To run:
//  server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

typedef struct{

    int workers;
    int buffer_max;
    int buffer_count;
    pthread_mutex_t* mutex;
    pthread_cond_t* empty;
    pthread_cond_t* fill;
    int fill_ptr;
    int use_ptr;
    int* buffer;
    pthread_t* threads;

}server_t;


// CS537: Parse the new arguments too
void getargs(int *port, int argc, char *argv[], int* workers, int* buffers)
{
    if (argc != 4) {
	fprintf(stderr, "Usage: %s <port> <threads> <buffers>\n", argv[0]);
	exit(1);
    }
    *port = atoi(argv[1]);
    *workers = atoi(argv[2]);
    *buffers = atoi(argv[3]);

    if(*workers <1  || *buffers < 1)
        exit(1);
}

void put(void* arg, int value){
    server_t* server = (server_t*) arg;
    server->buffer[server->fill_ptr] = value;
    server->fill_ptr = (server->fill_ptr +1) % server->buffer_max;
    server->buffer_count++;
}


int get(void* arg){
    server_t* server = (server_t*)arg;
    int tmp = server->buffer[server->use_ptr];
    server->use_ptr = (server->use_ptr +1) % server->buffer_max;
    server->buffer_count--;
    return tmp;

}

void *producer(void* arg, int value){
    server_t* server = (server_t*) arg;
    pthread_mutex_lock(server->mutex);
    while(server->buffer_count == server->buffer_max)
         pthread_cond_wait(server->empty, server->mutex);
    put(server, value);
    pthread_cond_signal(server->fill);
    pthread_mutex_unlock(server->mutex);
    

}

void* consumer(void* arg){
    server_t* server = (server_t*) arg;
    while(1){
        pthread_mutex_lock(server->mutex);
        while(server->buffer_count == 0)
            pthread_cond_wait(server->fill, server->mutex);
        int tmp = get(server);
        pthread_cond_signal(server->empty);
        pthread_mutex_unlock(server->mutex);

        requestHandle(tmp);
        Close(tmp);
    }
}

int main(int argc, char *argv[])
{
    int listenfd, connfd, port, clientlen;
    struct sockaddr_in clientaddr;

    server_t* server = (server_t*) malloc(sizeof(server_t));

    getargs(&port, argc, argv, &(server->workers), &(server->buffer_max));


    server -> buffer_count = 0;
    server -> fill_ptr = 0;
    server -> use_ptr = 0;
    server -> buffer = (int*) malloc(sizeof(int) * server->buffer_max);
    server -> threads = (pthread_t*) malloc(sizeof(pthread_t) * server->workers);
    server -> mutex = (pthread_mutex_t*) malloc(sizeof(pthread_mutex_t));
    server -> fill = (pthread_cond_t*) malloc(sizeof(pthread_cond_t));
    server -> empty = (pthread_cond_t*) malloc(sizeof(pthread_cond_t));
    // 
    // CS537: Create some threads...
    //

    for(int i = 0; i< server->workers; i++){
        pthread_create(&(server->threads[i]), NULL, consumer, server);


    }
    listenfd = Open_listenfd(port);
    while (1) {
	clientlen = sizeof(clientaddr);
	connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);

	// 
	// CS537: In general, don't handle the request in the main thread.
	// Save the relevant info in a buffer and have one of the worker threads 
	// do the work. 
	//

    producer(server, connfd);
	//requestHandle(connfd);

	//Close(connfd);
    }

}


    


 
