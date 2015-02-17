#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

// Used to track the names of start and end rooms
struct Positions{
	char *start;
	char *end;
	char *path;
};

char *createDirectory(int processID); // Returns name of rooms directory
struct Positions generateRooms(char *roomsDirectory); // Returns name of start room;
void shuffle(char **array, size_t n); // Shuffles room names when needed
void gameLoop(struct Positions pos); // Contains the actual game logic which handles the files
int search(char *file, char *str); // Searches files for matches

int main(void) {
    srand(time(NULL));
    int pid = getpid();
    char *roomsFolder = createDirectory(pid);    
    struct Positions pos = generateRooms(roomsFolder);
    // printf("START_ROOM is %s.\n", startRoom); // Debug
    gameLoop(pos);
    
    free(roomsFolder);
    return 0; 
}

void gameLoop(struct Positions pos) {
	char *currentRoom = pos.start;
	char *endRoom = pos.end;
	char *directory = pos.path;
	int stepCount = 0;
	char *steps[100]; // Tracks up to 100 steps
	int i; // For-loop controller
	char *contents[8]; // Stores connections for each file

	int bufSize = 128;
    char *currentFile = malloc(bufSize);

	while (currentRoom != endRoom) {
		// Open the current file for reading
		snprintf(currentFile, bufSize, "%s/%s", directory, currentRoom); // Concatenates path to 1 string
        FILE *f = fopen(currentFile, "r");

        int c, numCon;
        if (f) {
		    while ((c = getc(f)) != EOF){
		        putchar(c); // DEBUG
		        if (c == '\n') 
		        	numCon++;
		    }
		}
		printf("\n");

		numCon = numCon - 2; // Subtract room name and type to get # of connections
		//printf("numCon=%d\n", numCon); // debug

		// Get  current position
		char str[20];
		fseek(f, 11, SEEK_SET);
		fgets(str, 20, f);
		// remove newline
		int len = strlen(str);
		if(str[len-1] == '\n')
		    str[len-1] = 0;
		contents[0] = str;
		printf("Contents[0] = %s\n", contents[0]);

		// Extract connections and store in contents[]
		for(i = 1; i < (numCon+1); i++){
			fseek(f, 14, SEEK_CUR);
			fgets(str, 20, f);
			// remove newline
			len = strlen(str);
			if(str[len-1] == '\n')
			    str[len-1] = 0;
			contents[i] = str;
			printf("Contents[%d] = %s\n", i, contents[i]);
		}

		//for (i = 0; i < numCon + 2; i++) // Debug
		//	printf("contents[%d] = %s\n", i, contents[i]);


		// Print extracted info
		printf("CURRENT LOCATION: %s\n", contents[0]);


		
		printf("POSSIBLE CONNECTIONS: ");
		for(i = 1; i < (numCon+1); i++){
			if ((i + 1) == numCon) 
				printf("%s.\n", contents[i]);
			else printf("%s, ", contents[i]);
		}
		printf("\n");

        



        // Request input from user on next room to enter

		currentRoom = endRoom;
		fclose(f);
	}

	/*printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEPS. YOUR PATH TO VICTORY WAS:\n");
	for (i = 0; i < stepCount; i++) {
		printf("%s\n", steps[i]);
	}*/

	free(currentFile); // Deallocate memory
}

// This function generates a directory to store the rooms
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

// This function generates the "rooms" as files, as well as generating their properties
struct Positions generateRooms(char *roomsDirectory) {
	struct Positions pos;
	pos.path = roomsDirectory;
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
    //char *prefix = "room_";
    int i, j; // For-loop controllers

    shuffle(roomNames, 10); // Randomizes names for inconsistency between games

    for(i = 0; i < 7; i++) { // Create 7 files with unique names
        snprintf(currentFile, bufSize, "%s/%s", roomsDirectory, roomNames[i]); // Concatenates path to 1 string
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

    for(i = 0; i < 7; i++) { // Reopen 7 files to add random unique connections
    	shuffle(newNames, 7); // Shuffle the first 7 elements (only 7 needed each game)
        snprintf(currentFile, bufSize, "%s/%s", roomsDirectory, roomNames[i]); // Concatenates path to 1 string

        FILE *f = fopen(currentFile, "a"); // Example: "hinesro.rooms.12345/rooms_1". Opens in append mode.
        if (f == NULL) {
        	perror ("Error opening file.\n");
        } else {
            // each room needs 3-6 random non-repeating connections, not connected to self
            connectionCount = rand() % 4 + 3; // each room randomly has between 3 and 6 connections

            idx = 0;
            for (j = 0; j < connectionCount; j++) { // generate connections for each room
            	connection = newNames[idx];
            	if (connection == roomNames[i]) { // if connected to self, skip this index
            		idx++;
            		connection = newNames[idx];
            	}
            	fprintf(f, "CONNECTION %d: %s\n", j+1, connection);
            	idx++;
            }
            if (i == startPos) {
            	fprintf(f, "ROOM TYPE: START_ROOM\n");
            	pos.start = roomNames[i];
            }
            else if (i == endPos ) {
            	fprintf(f, "ROOM TYPE: END_ROOM\n");
            	pos.end = roomNames[i];
            } else fprintf(f, "ROOM TYPE: MID_ROOM\n");
        }
        fclose(f);
    }
    free(currentFile); // Deallocate memory
    return pos;
}

// This is used for shuffling my array of potential room names to ensure unique game play-throughs
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

int search(char *file, char *str) {
	FILE *f;
	int lineNum = 1;
	int numFound = 0;
	char temp[512];
	
	if((f = fopen(file, "r")) == NULL) {
		return(-1);
	}

	while(fgets(temp, 512, f) != NULL) {
		if((strstr(temp, str)) != NULL) {
			numFound++;
		}
		lineNum++;
	}

	//Close the file if still open.
	if(f) {
		fclose(f);
	}

	return numFound;
}