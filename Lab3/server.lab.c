#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //close()

#include <sys/socket.h> // socket(), connect(), send(), recv()
#include <arpa/inet.h> // sockaddr_in, inet_addr()

#define BUFSIZE 64
#define PENDING 16

void die(char *msg) {
    fprintf(stderr, "DEAD: %s\n", msg);
    exit(1);
}

void handle(int sock) {
    char buffer[BUFSIZE];
    int size;

    if ((size = recv(sock, buffer, BUFSIZE, 0)) < 0) {
        die("recv() failed");
    }

    if (strncmp(buffer, "dead", 4) == 0) { while (1); }

    while (size > 0) {
        if (send(sock, buffer, size, 0) != size) {
            die("send() failed");
        }
        if ((size = recv(sock, buffer, BUFSIZE, 0)) < 0) {
            die("recv() failed");
        }
    }
    close(sock);
}

int main(int argc, char* argv[]) {

    if (argc != 2) {
        fprintf(stderr, "Usage: %s <port num>", argv[0]);
        exit(1);
    }

    int sock;
    
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        die("socket() failed");
    }

    int port_num = atoi(argv[1]);
    struct sockaddr_in serv_addr;

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);
    serv_addr.sin_port = htons(port_num);

    if (bind(sock, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        die("bind() failed");
    }

    if (listen(sock, PENDING) < 0) {
        die("listen() failed");
    }

    struct sockaddr_in client_addr;
    unsigned int client_len;

    while (1) {
        int client_sock;
            
        if ((client_sock = accept(sock, (struct sockaddr *) &client_addr, &client_len)) < 0) {
            die("accept() failed");
        }
        printf("Handling client: %s\n", inet_ntoa(client_addr.sin_addr));
        handle(client_sock);
    }
}
