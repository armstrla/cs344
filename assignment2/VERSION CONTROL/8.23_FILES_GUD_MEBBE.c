#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

char *createDirectory(int processID);
void generateRooms(char *roomsDirectory);
void shuffle(char **array, size_t n);

int main(void) {
    srand(time(NULL));
    int pid = getpid();
    char *roomsFolder = createDirectory(pid);    
    generateRooms(roomsFolder);
    
    return 0; 
}

char *createDirectory(int processID) {
    int bufSize = 20;
    char *directoryName = malloc(bufSize);
    char *prefix = "hinesro.rooms.";

    snprintf(directoryName, bufSize, "%s%d", prefix, processID);

    struct stat st = {0};
    if (stat(directoryName, &st) == -1)
        mkdir(directoryName, 0755);

    return directoryName;
}

void generateRooms(char *roomsDirectory) {
    char *roomNames[10];
    roomNames[0] = "Gul";
    roomNames[1] = "Leint";
    roomNames[2] = "Venjo";
    roomNames[3] = "Falenidd";
    roomNames[4] = "Eowede";
    roomNames[5] = "Hapina";
    roomNames[6] = "Nystiael";
    roomNames[7] = "Prerramos";
    roomNames[8] = "Thedria";
    roomNames[9] = "Unith";

    int bufSize = 128;
    char *currentFile = malloc(bufSize);
    char *prefix = "room_";
    int i, j; // For-loop controllers

    shuffle(roomNames, 10); // Randomizes names for inconsistency between games

    for(i = 0; i < 7; i++) { // Create 7 files with unique names
        snprintf(currentFile, bufSize, "%s/%s%d", roomsDirectory, prefix, i+1); // Concatenates path to 1 string
        FILE *f = fopen(currentFile, "w");

        if (f == NULL) perror ("Error opening file.\n");
        else {
            fprintf(f, "ROOM NAME: %s\n", roomNames[i]); // each room now has a name
        }

        fclose(f);
    }

    char *newNames[7];
    for(i = 0; i < 7; i++) { // Store the chosen 7 in a new array to avoid accessing unchosen names
    	newNames[i] = roomNames[i];
    }

    // Determine some random start and end rooms. 
    int startPos = rand() % 7;
    int endPos = rand () % 7;
    while (startPos == endPos) endPos = rand() % 7;

    // Initialize variables to generate connections.
    char *connection;
    
    int connectionCount; // Will store number of connections per room, 3-6
    int idx; // Will be used to search names array for connections
    char currentRoom[100]; // initialize string to store what the current room is
    //char *room;

    for(i = 0; i < 7; i++) { // Reopen 7 files to add random unique connections
    	shuffle(newNames, 7); // Shuffle the first 7 elements (only 7 needed each game)
        snprintf(currentFile, bufSize, "%s/%s%d", roomsDirectory, prefix, i+1); // Concatenates path to 1 string
        FILE *f = fopen(currentFile, "r"); // Example: "hinesro.rooms.12345/rooms_1". Opens in read mode.

        if (f == NULL) {
        	perror ("Error opening file.\n");
        } else {
        	fseek(f, 11, SEEK_SET); // Find the actual string value after "ROOM NAME: ""
        	fgets(currentRoom, 100, f); // Copy this value into currentRoom
        	//room = strtok(currentRoom, "\n");
        	//printf("Current room is %s.\n", room); // for debug
        }
        fclose(f);

        f = fopen(currentFile, "a"); // Example: "hinesro.rooms.12345/rooms_1". Opens in append mode.
        if (f == NULL) {
        	perror ("Error opening file.\n");
        } else {
            // each room needs 3-6 random non-repeating connections, not connected to self
            connectionCount = rand() % 4 + 3; // each room randomly has between 3 and 6 connections

            idx = 0;
            for (j = 0; j < connectionCount; j++) { // generate connections for each room
            	connection = newNames[idx];
            	//printf("%s is not equal to %s.\n", connection, room); // debug
            	if (connection == roomNames[i]) { // if connected to self, skip this index
            		printf("Connection is same as room.\n"); // debug
            		idx++;
            		connection = newNames[idx];
            	}
        		//printf("CONNECTION %d: %s\n", j+1, connection); // debug
            	fprintf(f, "CONNECTION %d: %s\n", j+1, connection);
            	idx++;
            }
            if (i == startPos) fprintf(f, "ROOM TYPE: START_ROOM\n");
            else if (i == endPos ) fprintf(f, "ROOM TYPE: END_ROOM\n");
            else fprintf(f, "ROOM TYPE: MID_ROOM\n");
        }
        fclose(f);
    }
}

void shuffle(char **array, size_t n) {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    int usec = tv.tv_usec;
    srand48(usec);

    if (n > 1) {
        size_t i;
        for (i = n - 1; i > 0; i--) {
            size_t j = (unsigned int) (drand48()*(i+1));
            char* t = array[j];
            array[j] = array[i];
            array[i] = t;
        }
    }
}
