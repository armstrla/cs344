/****************************************************************************

otp_enc: 

This program connects to otp_enc_d, and asks it to perform a one-time pad 
style encryption as detailed above. By itself, otp_enc doesn’t do the 
encryption. Its syntax is as follows:

otp_enc plaintext key port

****************************************************************************/
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include <sys/stat.h> // File statistics

int main(int argc, char *argv[]) {
	// Ensure that correct arguments were used
	if (argc < 4) {
		printf("Usage: %s <plaintext-file> <key-file> <port-of-daemon>\n", argv[0]);
		return 1;
	}

	// Verify existence of plaintext and keyfile args
    char *ptfname = argv[1];
    char *keyfname = argv[2];

    if (access(ptfname, F_OK) == -1) {
		printf("Error: Unable to locate %s.\n", ptfname);
    	return 1;
	} else if (access(keyfname, F_OK) == -1) {
		printf("Error: Unable to locate %s.\n", keyfname);
    	return 1;
	}

    // Verify that keyfile is greater than or equal to plaintext
    struct stat st1, st2;
	stat(ptfname, &st1);
	stat(keyfname, &st2);
	int ptsize = st1.st_size;
	int keysize = st2.st_size;

    if (ptsize > keysize) {
    	printf("Error: Key not long enough.\n");
    	return 1;
    }

    /* Socket communication */
    int sockfd = 0, n = 0; // Initialize socket file descriptor and read controller
    int port = atoi(argv[1]); // Grab port arg and convert to int
    char recvBuff[1024]; // Buffer for text that is received
    char sendBuff[1024]; // Buffer for text that is sent
    struct sockaddr_in serv_addr;  // Info for connecting to server

    memset(recvBuff, '0',sizeof(recvBuff)); // Clear receive buffer
    memset(sendBuff, '0',sizeof(sendBuff)); // Clear send buffer

    // Create the socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("Error : Could not create socket.\n");
        return 1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr)); // Clear server address

    // Establish server settings
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port); 
    serv_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    // Attempt to connect to the server using the specified port
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    	printf("Error: No one was listening on port %d.\n", port);
    	return 1;
    }

    snprintf(sendBuff, 1024, ptfname);

    // TODO: Pass along filenames (plaintext and key) to the daemon and wait for ciphertext response
    // TODO: copy these into the send buffer
    n = write(sockfd, sendBuff, strlen(sendBuff));
    if (n < 0)
        printf("Error: Writing to socket failed.\n");

    memset(sendBuff, '0',sizeof(sendBuff)); // Clear send buffer
    snprintf(sendBuff, 1024, keyname);

    n = write(sockfd, sendBuff, strlen(sendBuff));
    if (n < 0)
        printf("Error: Writing to socket failed.\n");
    
    // TODO: Print ciphertext


    // Read from the socket connection until the return value is 0
    //while ((n = read(sockfd, recvBuff, sizeof(recvBuff) - 1)) > 0) {
    //    recvBuff[n] = 0;
    //    if (fputs(recvBuff, stdout) == EOF) {
    //        printf("Error : Fputs error.\n");
    //    }
    //}

    //if (n < 0) {
    //    printf("Read error.\n");
    //} 

    return 0;
}