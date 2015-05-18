#include <stdio.h> 
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <ctype.h> // letter to number conversion

#define BUFSIZE 1024 // Controls the length of all string buffers

// Prints error message and exits with failure status
void error(const char *msg) {
    perror(msg);
    exit(1);
}

// Converts char values to corresponding integers
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

// Checks to make sure input characters are valid
// (only A-Z and the space character)
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
    int sockfd, acceptfd, portno; // Initialize socket file descriptors, port
    socklen_t clilen; // Initialize client address length
    char recvbuf[BUFSIZE], sendbuf[BUFSIZE]; // Initialize send/receive string buffers
    struct sockaddr_in serv_addr, cli_addr; // Initialize server and client structs
    int n, i; // For-loop iterators

    if (argc < 2) { // Check for proper args usage
        fprintf(stderr,"Usage: %s port\n", argv[0]);
        exit(1);
    }

    pid_t pid, sid; // Initialize process ID and Session ID
    
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
    sockfd = socket(AF_INET, SOCK_STREAM, 0); // Set socket file descriptor for IP, TCP
    if (sockfd < 0) {
        error("ERROR opening socket.\n");
    }

    bzero((char *) &serv_addr, sizeof(serv_addr)); // Clear buffer for server address
    portno = atoi(argv[1]); // Save port number that was passed in, converting from string to int
    serv_addr.sin_family = AF_INET; // Establish address family as internet
    serv_addr.sin_addr.s_addr = INADDR_ANY; // Accept connections from any machine
    serv_addr.sin_port = htons(portno); // Only listen on the user-specified port

    // Bind server to socket using aforementioned configuration
    if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("ERROR on binding.\n");
    }

    // Listen for incoming connections (up to 5) on this socket
    listen(sockfd, 5);
    clilen = sizeof(cli_addr); // Grab length of client address to pass into accept call

    // Start the daemon/server listening loop
    while (1) {
        // Wait for connection attempts from client processes
        acceptfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
        if (acceptfd < 0) {
            error("ERROR on accept.\n");
        }

        /////* READ FROM CLIENT MESSAGE #1 */////
        bzero(recvbuf, BUFSIZE); // Clear the string buffer for received messages
        n = read(acceptfd, recvbuf, BUFSIZE); // Read any incoming messages
        if (n < 0) {
            error("ERROR reading from socket.\n");
        }

        // Attempt to open a file whose name was in the message
        FILE *fp;
        fp = fopen(recvbuf, "r");
        if (fp == NULL) {
            error("ERROR opening plaintext file.\n");
        }

        char ctxt[BUFSIZE]; // This will store ciphertext string to be extracted from file
        fgets(ctxt, BUFSIZE, fp); // Read file, store into ctxt

        for (i = 0; i < strlen(ctxt); i++) { // Convert ciphertext to all upper case (if needed)
            ctxt[i] = toupper(ctxt[i]);
        }

        for (i = 0; i < strlen(ctxt)-1; i++) { // Check for bad input
            if (charCheck(ctxt[i]) == 1) { // If charCheck returns true, exit with error
                error("ERROR ciphertext file contained bad characters.\n");
            }
        }

        fclose(fp);

        /////* READ FROM CLIENT MESSAGE #2 */////
        bzero(recvbuf, BUFSIZE); // Clear the buffer so it can be safely reused
        n = read(acceptfd, recvbuf, BUFSIZE); // Read any incoming messages into the buffer
        if (n < 0) {
            error("ERROR reading from socket.\n");
        }

        // Attempt to open the file named in the message
        fp = fopen(recvbuf, "r");
        if (fp == NULL) {
            error("ERROR opening key file.\n");
        }

        char ktxt[BUFSIZE]; // Stores key string to be extracted from file
        fgets(ktxt, BUFSIZE, fp); // Read file, store contents into ktxt

        fclose(fp);

        /////* USE KTXT TO ENCRYPT PTXT */////
        // First, ensure that ktxt is big enough for ctxt
        if (strlen(ktxt) < strlen(ctxt)) {
            error("ERROR key is shorter than ciphertext.\n");
        }

        // Then dencrypt each char and push to ptxt
        char ptxt[strlen(ctxt)]; // This will store the plaintext once generated
        // These int arrays will store the messages converted into numbers, and their modular sums
        int cnums[strlen(ctxt)], knums[strlen(ctxt)], pnums[strlen(ctxt)];
        char alpha[28] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ "; // This will be used to reference alphabetical positions

        for (i = 0; i < strlen(ctxt); i++) {    // Iterate through each character in the ciphertext
            cnums[i] = charToInt(ctxt[i]);      // Each index of cnums gets the numeric equivalent of ctxt
            knums[i] = charToInt(ktxt[i]);      // Each index of knums gets the numeric equivalent of ktxt
            pnums[i] = cnums[i] - knums[i];     // Each index of pnums gets the difference of cnums-knums
            if (pnums[i] < 0) {                 // Modular subtraction if the pnum is less than zero
                pnums[i] += 27;
            }
            ptxt[i] = alpha[pnums[i]];          // Set ptxt[i] equal to the corresponding resulting letter
        }
        ptxt[strlen(ctxt)-1] = '\0'; // Null terminate the resulting message

        ////* WRITE BACK TO THE CLIENT *////
        bzero(sendbuf, BUFSIZE); // Clear the send buffer so it can be used safely
        snprintf(sendbuf, BUFSIZE, "%s", ptxt); // Store the cipher string into the buffer
        n = write(acceptfd, sendbuf, strlen(sendbuf)); // Write this message back to the client
        if (n < 0) {
            error("ERROR writing to socket.\n");
        }
    }
    return 0;
}
