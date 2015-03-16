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

int charCheck(char c) {
    switch (c) {
        case 'A': return 0;
        case 'B': return 0;
        case 'C': return 0;
        case 'D': return 0;
        case 'E': return 0;
        case 'F': return 0;
        case 'G': return 0;
        case 'H': return 0;
        case 'I': return 0;
        case 'J': return 0;
        case 'K': return 0;
        case 'L': return 0;
        case 'M': return 0;
        case 'N': return 0;
        case 'O': return 0;
        case 'P': return 0;
        case 'Q': return 0;
        case 'R': return 0;
        case 'S': return 0;
        case 'T': return 0;
        case 'U': return 0;
        case 'V': return 0;
        case 'W': return 0;
        case 'X': return 0;
        case 'Y': return 0;
        case 'Z': return 0;
        case ' ': return 0;
        default : return 1;
    }
}

int main(int argc, char *argv[]) {
    int sockfd, acceptfd, portno;
    socklen_t clilen;
    char recvbuf[BUFSIZE], sendbuf[BUFSIZE];
    struct sockaddr_in serv_addr, cli_addr;
    int n, i;

    if (argc < 2) { // Check for proper args usage
        fprintf(stderr,"Usage: %s port\n", argv[0]);
        exit(1);
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


    // Initialize daemon
    sockfd = socket(AF_INET, SOCK_STREAM, 0);

    if (sockfd < 0) {
        error("ERROR opening socket.\n");
        exit(1);
    }

    bzero((char *) &serv_addr, sizeof(serv_addr));
    portno = atoi(argv[1]);
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_addr.s_addr = INADDR_ANY;
    serv_addr.sin_port = htons(portno);

    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR on binding.\n");
        exit(1);
    }

    listen(sockfd, 5);
    clilen = sizeof(cli_addr);

    // Start the daemon/server listening loop
    while (1) {
        acceptfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);

        if (acceptfd < 0) {
            error("ERROR on accept.\n");
            exit(1);
        }

    /////* READ FROM CLIENT MESSAGE #1 */////
        bzero(recvbuf, BUFSIZE);
        n = read(acceptfd, recvbuf, BUFSIZE);
        if (n < 0) {
            error("ERROR reading from socket.\n");
            exit(1);
        }
        //printf("Server: Received first message \"%s\"\n", recvbuf);

        // Open file named in message
        FILE *fp;
        fp = fopen(recvbuf, "r");
        if (fp == NULL) {
            error("ERROR opening plaintext file.\n");
            exit(1);
        } //else {
            //printf("Server: Successfully opened file.\n");
        //}

        char ptxt[BUFSIZE]; // stores plaintext string to be extracted from file
        fgets(ptxt, BUFSIZE, fp); // read file, store in ptxt

        //i = 0;
        //while (ptxt[i]) { // convert plaintext to all upper case
        //    ptxt[i] = toupper(ptxt[i]);
        //    i++;
        //}

        for (i = 0; i < strlen(ptxt); i++) { // convert plaintext to all upper case
            ptxt[i] = toupper(ptxt[i]);
        }

        for (i = 0; i < strlen(ptxt)-1; i++) { // check for bad input
            if (charCheck(ptxt[i]) == 1) { // If charCheck returns true, exit with error
                printf("ERROR plaintext file contained bad characters.\n");
                exit(1);
            }
        }

        // Check for bad input
        //i = 0;
        //while (ptxt[i]) { 
        //    if (charCheck(ptxt[i]) == 1) { // If charCheck returns true, exit with error
        //        printf("ERROR plaintext file contained bad characters\n");
        //        exit(1);
        //    }
        //}

        //printf("Server: Extracted from file:\n%s", ptxt);

        fclose(fp);

    /////* READ FROM CLIENT MESSAGE #2 */////
        bzero(recvbuf, BUFSIZE);
        n = read(acceptfd, recvbuf, BUFSIZE);
        if (n < 0) {
            error("ERROR reading from socket.\n");
            exit(1);
        }
        //printf("Server: Received second message \"%s\"\n", recvbuf);

        // Open file named in message
        //FILE *fp;
        fp = fopen(recvbuf, "r");
        if (fp == NULL) {
            error("ERROR opening key file.\n");
            exit(1);
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
            exit(1);
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
        //printf("sendbuf: %s\n", sendbuf);
        n = write(acceptfd, sendbuf, strlen(sendbuf));
        if (n < 0) error("ERROR writing to socket.\n");
        //write(sockfd, sendbuf, strlen(sendbuf));

        //close(acceptfd);
        //close(sockfd);
    }

    return 0; 
}