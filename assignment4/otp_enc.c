#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

#define BUFSIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int main(int argc, char *argv[]) {
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    char buffer[BUFSIZE];

    if (argc < 4) {
       fprintf(stderr,"Usage: %s plaintext key port\n", argv[0]);
       exit(1);
    }

    portno = atoi(argv[3]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        error("ERROR opening socket.\n");
    }
    
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(portno);
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    if (connect(sockfd,(struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        printf("ERROR connecting to port %d.\n", portno);
        exit(2);
    }

    /* SEND PLAINTEXT FILE NAME */
    //bzero(buffer,BUFSIZE);
    snprintf(buffer, BUFSIZE, "%s", argv[1]);
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) {
        error("ERROR writing argv[1] to socket.\n");
    }

    sleep(1);

    /* SEND KEY FILE NAME */
    snprintf(buffer, BUFSIZE, "%s", argv[2]);
    n = write(sockfd, buffer, strlen(buffer));
    if (n < 0) {
        error("ERROR writing to socket.\n");
    }

    /* RECEIVE SERVER RESPONSE */
    bzero(buffer, BUFSIZE);
    n = read(sockfd, buffer, BUFSIZE); // Waiting for server response
    if (n < 0) {
        error("ERROR reading from socket.\n");
    }

    if (buffer == "error1: key too short") {
        error("ERROR Key not long enough.\n");
    } else if (buffer == "error2") {
        error("ERROR Bad characters in file.\n");
    } else {
        printf("%s\n", buffer);
    }

    close(sockfd);
     
    return 0;
}