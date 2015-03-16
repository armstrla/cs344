#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h> // letter to number conversion

#define BUFSIZE 1024

void error(const char *msg) {
    perror(msg);
    exit(1);
}

int charToInt(char c) {
    switch (c) {
        case 'A': return 0;
        case 'B': return 1;
        case 'C': return 2;
        case 'D': return 3;
        case 'E': return 4;
        case 'F': return 5;
        case 'G': return 6;
        case 'H': return 7;
        case 'I': return 8;
        case 'J': return 9;
        case 'K': return 10;
        case 'L': return 11;
        case 'M': return 12;
        case 'N': return 13;
        case 'O': return 14;
        case 'P': return 15;
        case 'Q': return 16;
        case 'R': return 17;
        case 'S': return 18;
        case 'T': return 19;
        case 'U': return 20;
        case 'V': return 21;
        case 'W': return 22;
        case 'X': return 23;
        case 'Y': return 24;
        case 'Z': return 25;
        case ' ': return 26;
    }
}

int main(int argc, char *argv[]) {
    int sockfd, acceptfd, portno;
    socklen_t clilen;
    char recvbuf[BUFSIZE], sendbuf[BUFSIZE];
    struct sockaddr_in serv_addr, cli_addr;
    int n, i;

    if (argc < 2) {
        fprintf(stderr,"ERROR, no port provided.\n");
        exit(1);
    }

    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) 
        error("ERROR opening socket.\n");

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR on binding.\n");

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    acceptfd = accept(sockfd, (struct sockaddr *) &cli_addr, 
        &clilen);

    if (acceptfd < 0)
        error("ERROR on accept.\n");

/////* READ FROM CLIENT MESSAGE #1 */////
    bzero(recvbuf, BUFSIZE);
    n = read(acceptfd, recvbuf, BUFSIZE);
    if (n < 0) error("ERROR reading from socket.\n");
    //printf("Server: Received first message \"%s\"\n", recvbuf);

    // Open file named in message
    FILE *fp;
    fp = fopen(recvbuf, "r");
    if (fp == NULL) {
        error("ERROR opening plaintext file.\n");
        return(-1);
    } //else {
        //printf("Server: Successfully opened file.\n");
    //}

    char ptxt[BUFSIZE]; // stores plaintext string to be extracted from file
    fgets(ptxt, BUFSIZE, fp); // read file, store in ptxt

    while (ptxt[i]) { // convert plaintext to all upper case
        ptxt[i] = toupper(ptxt[i]);
        i++;
    }

    //printf("Server: Extracted from file:\n%s", ptxt);

    fclose(fp);

/////* READ FROM CLIENT MESSAGE #2 */////
    bzero(recvbuf, BUFSIZE);
    n = read(acceptfd, recvbuf, BUFSIZE);
    if (n < 0) error("ERROR reading from socket.\n");
    //printf("Server: Received second message \"%s\"\n", recvbuf);

    // Open file named in message
    //FILE *fp;
    fp = fopen(recvbuf, "r");
    if (fp == NULL) {
        error("ERROR opening key file.\n");
        return(-1);
    } //else {
        //printf("Server: Successfully opened file.\n");
    //}

    char ktxt[BUFSIZE]; // stores plaintext string to be extracted from file
    fgets(ktxt, BUFSIZE, fp); // read file, store in ktxt

    //printf("Server: Extracted from file:\n%s", ktxt);
    //n = write(acceptfd,"Received this plaintext:\n", 18);
    //write(acceptfd, ktxt, strlen(ktxt));
    fclose(fp);

/////* USE KTXT TO ENCRYPT PTXT */////
    // First, ensure that ktxt is big enough for ptxt
    if (strlen(ktxt) < strlen(ptxt)) {
        error("ERROR key is shorter than plaintext.\n");
        return(-1);
    }

    // Then encrypt each char and push to ctxt
    char ctxt[strlen(ptxt)];
    int pnums[strlen(ptxt)], knums[strlen(ptxt)], cnums[strlen(ptxt)];
    char alpha[28] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ ";

    for (i = 0; i < strlen(ptxt); i++) {    // Iterate through each character in the plaintext
        pnums[i] = charToInt(ptxt[i]);      // Each index of pnums gets the numeric equivalent of ptxt
        knums[i] = charToInt(ktxt[i]);      // Each index of knums gets the numeric equivalent of ktxt
        cnums[i] = pnums[i] + knums[i];     // Each index of cnums gets the sum if pnums+knums
        if (cnums[i] > 26) {                // Modular addition if the cnum is over 26 (zero is 'A')
            cnums[i] -= 26;
        }
        ctxt[i] = alpha[cnums[i]];          // Set ctxt[i] equal to the corresponding resulting letter
    }

////* WRITE BACK TO THE CLIENT *////
    bzero(sendbuf, BUFSIZE);
    snprintf(sendbuf, BUFSIZE, "%s", ctxt);
    printf("sendbuf: %s\n", sendbuf);
    n = write(acceptfd, sendbuf, strlen(sendbuf));
    if (n < 0) error("ERROR writing to socket.\n");
    //write(sockfd, sendbuf, strlen(sendbuf));

    close(acceptfd);
    close(sockfd);
    return 0; 
}