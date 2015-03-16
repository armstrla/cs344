/****************************************/
/*	otp_enc_d: ("D" STANDS FOR DAEMON)	*/
/*										*/
/* 	Example usage: 						*/
/*										*/
/*		otp_enc_d 57171 &				*/
/****************************************/
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <time.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <syslog.h>

int main(int argc, char *argv[]) {
	// Check for port argument
	if (argc < 2) {
		printf("Usage: %s <listening-port>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

    pid_t pid, sid; // Our process ID and Session ID
    
    // Fork off the parent process
    pid = fork();
    if (pid < 0) {
        exit(EXIT_FAILURE);
    }

    // If PID is good, exit parent process
    if (pid > 0) {
        exit(EXIT_SUCCESS);
    }

    // Change file mode mask to ensure daemon can read/write files
    umask(0);   

    // Create new SID for child process
    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }

    // Close out the standard file descriptors (daemon shouldn't have access to terminal)
    close(STDIN_FILENO);
    close(STDOUT_FILENO);
    close(STDERR_FILENO);

    /* DAEMON LOGIC OVER--START SOCKET LOGIC */
    int port = atoi(argv[1]); // Grab port from args
    int listenfd = 0, connfd = 0, n = 0; // Initialize file descriptors
    struct sockaddr_in serv_addr; // Declare server struct
    char recvBuff[1024]; // Buffer for text that is received
    char sendBuff[1024]; // Buffer for text that is sent

    // Create socket as IP, TCP
    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    memset(&serv_addr, '0', sizeof(serv_addr)); // Clear server address
    //memset(sendBuff, '0', sizeof(sendBuff));  // Clear the send buffer
    //memset(recvBuff, '0', sizeof(recvBuff));  // Clear the send buffer

    // Configure server settings
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY); // Accept connections from any IP
    serv_addr.sin_port = htons(port); // Listen on user-specified port

    bind(listenfd, (struct sockaddr*)&serv_addr, sizeof(serv_addr)); 

    listen(listenfd, 10); 

    while(1) {
        connfd = accept(listenfd, (struct sockaddr*)NULL, NULL);
        if (connfd < 0) perror("Error: Couldn't accept.\n");

        // Read from client
	    bzero(recvBuff, 1024);
	    n = read(newsockfd, buffer, 255);
	    if (n < 0) perror("Error: Couldn't read from socket.\n");
	    //printf("Here is the message: %s\n", buffer);
	    char *ptfname = recvBuff;
	    FILE *fp;
		if ((fp = fopen(ptfname,"r")) == NULL) { 
			perror("Error: File not found.\n"); 
			exit(1);
		}
		// Store contents of file into local string



		fclose(fp);

        //ticks = time(NULL);
        //snprintf(sendBuff, sizeof(sendBuff), "%.24s\r\n", ctime(&ticks));
        //write(connfd, sendBuff, strlen(sendBuff));

        // Read from the socket connection until the return value is 0
	    while ((n = read(sockfd, recvBuff, sizeof(recvBuff) - 1)) > 0) {
	        recvBuff[n] = 0;
	        if (fputs(recvBuff, stdout) == EOF) {
	            perror("Error: Fputs error.\n");
	        }
	    }

	    if (n < 0) {
	        perror("Read error.\n");
	    } 

        close(connfd);
        sleep(1);
    }

	return 0;
}