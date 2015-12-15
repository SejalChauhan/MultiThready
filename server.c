#include "cs537.h"
#include "request.h"
#include <pthread.h>


int count_proc_barray;
int proc_barraylocation;
int cons_barraylocation;
pthread_cond_t empty,fill;
pthread_mutex_t m;
              
struct cons_arg{
    int numbuffers;
    int *buff;
};
// server.c: A very, very simple web server
//
// To run:
//  server <portnum (above 2000)>
//
// Repeatedly handles HTTP requests sent to this port number.
// Most of the work is done within routines written in request.c
//

// CS537: Parse the new arguments too
void getargs(int *port, int *numthreads, int *numbuffers, int argc, char *argv[])
{
    if (argc != 4) {
	fprintf(stderr, "Usage: %s <port> %s <threads> %s <buffer>\n", argv[0], argv[1], argv[2]);
	exit(1);
    }

    *port = atoi(argv[1]);
    *numthreads = atoi(argv[2]);
    *numbuffers = atoi(argv[3]);
    if (*port<0 || *numthreads<0 || *numbuffers<0){
       exit(1);
    }
    fprintf(stderr, "Final: %d <port> %d <threads> %d <buffers>\n", *port, *numthreads, *numbuffers);
    
}


//producer
void *producer(int *buff, int numbuffers, int port)
{
    int listenfd, clientlen, connfd;
    struct sockaddr_in clientaddr;

    listenfd = Open_listenfd(port);

    while (1) {
        clientlen = sizeof(clientaddr);
        connfd = Accept(listenfd, (SA *)&clientaddr, (socklen_t *) &clientlen);
        pthread_mutex_lock(&m);
        while(count_proc_barray==numbuffers){
            printf("%s %d\n", __func__, __LINE__);
            pthread_cond_wait(&empty, &m);
            }
        //call the request handle from here

    
        printf("connfd %s %d\n", __func__, connfd);
        buff[proc_barraylocation]= connfd;
        printf("%s bufferloc %d numbuffers %d\n", __func__, proc_barraylocation, numbuffers);
        printf("%s %d\n", __func__, __LINE__);
        proc_barraylocation++;
        if (proc_barraylocation==numbuffers)
            proc_barraylocation=0;
        count_proc_barray++;
        pthread_cond_signal(&fill);
        pthread_mutex_unlock(&m);
    }
}


//consumer
void *consumer(void *arg)
{
    int connfd;
    struct cons_arg *ptr = (struct cons_arg *)arg;


    while(1){
        printf("%s %d\n", __func__, __LINE__);
        pthread_mutex_lock(&m);
        while(count_proc_barray == 0){
            printf("%s %d\n", __func__, __LINE__);
            pthread_cond_wait(&fill, &m);
            }
        printf("%s %d\n", __func__, __LINE__);
        printf("%s bufferloc %d numbuffers %d\n", __func__, cons_barraylocation, ptr->numbuffers);
        connfd = ptr->buff[cons_barraylocation];
        cons_barraylocation++;
        if (cons_barraylocation== ptr->numbuffers)
            cons_barraylocation=0;
        printf("connfd %s %d\n", __func__, connfd);
        printf("%s %d\n", __func__, __LINE__);
        count_proc_barray--;
        pthread_cond_signal(&empty);
        pthread_mutex_unlock(&m);
        printf("%s %d\n", __func__, __LINE__);
        requestHandle(connfd);
        printf("%s %d\n", __func__, __LINE__);
        Close(connfd);
        printf("%s %d\n", __func__, __LINE__);
    }

}


int main(int argc, char *argv[])
{
    int port, numthreads, numbuffers;
    
    int i;
    int *buffer_arr;
    pthread_t *thread_arr;
    struct cons_arg ptr;

    getargs(&port, &numthreads, &numbuffers, argc, argv);

    // 
    // CS537: Create some threads...
    //

    thread_arr = malloc(numthreads*sizeof(pthread_t));
    buffer_arr = malloc(numbuffers*sizeof(int));

    pthread_cond_init(&empty, NULL);
    pthread_cond_init(&fill, NULL);
    pthread_mutex_init(&m, NULL);

	// 
	// CS537: In general, don't handle the request in the main thread.
	// Save the relevant info in a buffer and have one of the worker threads 
	// do the work. However, for SFF, you may have to do a little work
	// here (e.g., a stat() on the filename) ...
	// 
    ptr.buff=buffer_arr;
    ptr.numbuffers=numbuffers;
    for (i=0;i<numthreads;i++){
        pthread_create(&thread_arr[i], NULL, consumer, (void *)&ptr );
    }
    producer(buffer_arr, numbuffers, port);

    for (i=0;i<numthreads;i++){
        pthread_join(thread_arr[i],NULL);
    }
    
    free(thread_arr);
    free(buffer_arr);

}


    


 
