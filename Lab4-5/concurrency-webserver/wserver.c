#include <stdio.h>
#include <pthread.h>

#include "request.h"
#include "io_helper.h"
#include "thread_pool.h"

void *mythread(void* conn_fd) {
	int connection = (int)conn_fd;
	request_handle(connection);
	close_or_die(connection);
	return NULL;
}

char default_root[] = ".";

//
// ./wserver [-d <basedir>] [-p <portnum>] 
// 
int main(int argc, char *argv[]) {
    int c;
    char *root_dir = default_root;
    int port = 10000;
	int threadNum = 1;
	int buffer = 1;
	char* schedalg = "FIFO"; 
    
    while ((c = getopt(argc, argv, "d:p:t:b:s")) != -1)
		switch (c) {
			case 'd':
				root_dir = optarg;
				break;
			case 'p':
				port = atoi(optarg);
				break;
			case 't':
				threadNum = atoi(optarg);
				if (threadNum <= 0) {
					exit(-1);
				}
				break;
			case 'b':
				buffer = atoi(optarg);
				if (buffer <= 0) {
					exit(-1);
				}
				break;
			case 's':
				if (strcmp("FIFO", optarg) == 0) { schedalg = "FIFO"; }
				else if (strcmp("SFF", optarg) == 0) { schedalg = "SFF"; }
				else { exit(-1); }
				break;
			
			default:
				fprintf(stderr, "usage: wserver [-d basedir] [-p port] [-t threads] [-b buffer] [-s scheduling algorithm\n");
				exit(1);
			}
			
 	printf("Thread Number: %d\nBuffer Number: %d\nScheduling Algorithm: %s\n", threadNum, buffer, schedalg);   

    // run out of this directory
    chdir_or_die(root_dir);

    // now, get to work
    int listen_fd = open_listen_fd_or_die(port);

	thread_pool_init(threadNum); // Initialize thread pool

    while (1) {
		struct sockaddr_in client_addr;
		int client_len = sizeof(client_addr);
		int conn_fd = accept_or_die(listen_fd, (sockaddr_t *) &client_addr, (socklen_t *) &client_len);
		while (Queue_Empty(&jobs) == 1) { // While queue is empty, enqueue connections
			Queue_Enqueue(&jobs, conn_fd);
		}
		// After queue is full, unlock thread and empty queue
		if (pthread_cond_signal(&fill) == 0) {
			Empty_Queue(&jobs);
		}
	}
    return 0;
}


    


 
