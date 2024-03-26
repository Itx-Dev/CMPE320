#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h> //close()

#include <sys/socket.h> // socket(), connect(), send(), recv()
#include <arpa/inet.h> // sockaddr_in, inet_addr()

#define BUFSIZE 64

void die(char *msg) {
    fprintf(stderr, "DEAD: %s\n", msg);
    exit(1);
}


int main(int argc, char *argv[]) {

    if (argc != 4) {
        fprintf(stderr, "Usage: %s <server ip> <port num> <message>", argv[0]);
        exit(1);
    }

    char *server_ip = argv[1];
    int port_num = atoi(argv[2]);
    char *msg = argv[3];

    int sock;
    
    if ((sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) < 0) {
        die("socket() failed");
    }

    struct sockaddr_in s_addr;
    s_addr.sin_family = AF_INET;
    s_addr.sin_addr.s_addr = inet_addr(server_ip);
    s_addr.sin_port = htons(port_num);

    connect(sock, (struct sockaddr *) &s_addr, sizeof(s_addr));
    
    int msg_len = strlen(msg);

    /* Send the message to the server */
    if (send(sock, msg, msg_len, 0) != msg_len) {
        die("Message failed to send");
    }

    int total_recv = 0;
    char buffer[BUFSIZE];

    while (total_recv < msg_len) { // counting how many bytes received from the server
        int rcvd;

        if ((rcvd = recv(sock, buffer, BUFSIZE-1, 0)) <= 0) {
            die("recv() failed or connection closed"); // read the man page
        }

        total_recv += rcvd;
        buffer[rcvd] = '\0';
        printf("%s", buffer);
    }

    close(sock);
    return 0;
}