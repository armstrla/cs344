#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>

// Used to pass multiple strings between functions
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
	// Initializes random seed for random number generation
    srand(time(NULL));

    // Grabs the current process ID to be used in the directory name
    int pid = getpid();

    // Create a directory and return its pathname
    char *roomsFolder = createDirectory(pid);    

    // Generate all room files and their contents
    struct Positions pos = generateRooms(roomsFolder);

    // Run the game loop on the generated files
    gameLoop(pos);
    
    // Free dynamically allocated memory returned from createDirectory
    free(roomsFolder);

    // Exit without error flag
    return 0; 
}

void gameLoop(struct Positions pos) {
	char *currentRoom = pos.start; // Initialize current room
	char *endRoom = pos.end; // Initialize end room
	char *directory = pos.path; // Grab filepath from Position object
	int stepCount = 0; // Initialize total number of steps player will make
	int i; // For-loop controller
	int valid; // boolean for checking if input is valid
	int bufSize = 128; // Buffer for pathname string
	int c, numCon; // Used to scan each char of file and track instances of newline chars
	char (*steps)[15] = malloc(sizeof *steps * 8); // stores names of every room accessed
	char (*contents)[15] = malloc(sizeof *contents * 8); // stores the contents for a given file
	char destination[15]; // User input for desired room
    char *currentFile = malloc(bufSize); // stores the complete filepath for the current room

	while (!(strcmp(currentRoom, endRoom)) == 0) {
		snprintf(currentFile, bufSize, "%s/%s", directory, currentRoom); // Concatenates path to 1 string
        FILE *f = fopen(currentFile, "r"); // Open the current file for reading

        numCon = 0; // Initialize the rooms number of connections at zero

        // Scan each character of the file for newline characters in order to get number of lines
        if (f) {
		    while ((c = getc(f)) != EOF){
		        if (c == '\n') 
		        	numCon++;
		    }
		}

		numCon = numCon - 2; // Using # of lines, subtract 2 for room name and type for total connections

		// Get  current position
		char str[20]; // initialize temporary holder for all room names
		fseek(f, 11, SEEK_SET); // Set the file pointer 11 chars in, just after "ROOM NAME:"
		fgets(str, 20, f); // Grab any text on the rest of the line to extract current room
		// remove newline from extracted string (makes everything easier later on)
		int len = strlen(str);
		if(str[len-1] == '\n') 
		    str[len-1] = 0;
		strcpy(contents[0], str);

		// Extract connections and store in contents[]
		for(i = 1; i <= numCon; i++){
			fseek(f, 14, SEEK_CUR); // Move pointer to right after "CONNECTION [#]: on next line
			fgets(str, 20, f); // Grab the text
			// remove newline
			len = strlen(str);
			if(str[len-1] == '\n')
			    str[len-1] = 0;
			strcpy(contents[i], str);
		}

		valid = 0; // Initialize valid flag at false

		while(valid == 0) {
			// Print extracted info
			printf("CURRENT LOCATION: %s\n", contents[0]);
			printf("POSSIBLE CONNECTIONS: ");
			for(i = 1; i <= numCon; i++){
				if (i == numCon) 
					printf("%s.\n", contents[i]); // Text formatting to match requirements
				else printf("%s, ", contents[i]);
			}

	        // Request input from user on next room to enter
	        printf("WHERE TO? ");
			scanf("%s", destination); // store the user's input in destination
			for(i = 1; i <= numCon; i++){ // Run through all connections and look for a text match
				if (strcmp(destination, contents[i]) == 0) { // If found, set currentRoom to this connection
					valid = 1;
					currentRoom = destination;
				}
			}
			if (valid != 1) // If not found, deliver error message and rerun loop
				printf("\nHUH? I DON’T UNDERSTAND THAT ROOM. TRY AGAIN.\n\n");
		}
		printf("\n");
		strcpy(steps[stepCount], currentRoom); // Save the next room in user's history
		stepCount++; // increment counter for total successful steps taken
		fclose(f);
	}

	// Once the player has broken out of this loop, they have found the end room and won the game
	printf("YOU HAVE FOUND THE END ROOM. CONGRATULATIONS!\n");
	printf("YOU TOOK %d STEP(S). YOUR PATH TO VICTORY WAS:\n", stepCount);
	for (i = 0; i < stepCount; i++) {
		printf("%s\n", steps[i]);
	}

	// Deallocate memory
	free(steps);
	free(contents);
	free(currentFile); 
}

// This function generates a directory to store the rooms
char *createDirectory(int processID) {
    int bufSize = 20;
    char *directoryName = malloc(bufSize);
    char *prefix = "hinesro.rooms.";

    // Concatenate all pieces of the directory name into one string
    snprintf(directoryName, bufSize, "%s%d", prefix, processID);

    // If directory with same name doesn't already exist, create it
    struct stat st = {0};
    if (stat(directoryName, &st) == -1)
        mkdir(directoryName, 0755);

    return directoryName;
}

// This function generates the "rooms" as files, as well as generating their properties
struct Positions generateRooms(char *roomsDirectory) {
	struct Positions pos; // Stores pathname, start position and end position
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

    int bufSize = 128; // Stromg buffer for memory allocation
    char *currentFile = malloc(bufSize);
    int i, j; // For-loop controllers

    shuffle(roomNames, 10); // Randomizes names for inconsistency between games

    for(i = 0; i < 7; i++) { // Grab the first 7, create 7 files with unique names 
        snprintf(currentFile, bufSize, "%s/%s", roomsDirectory, roomNames[i]); // Concatenates path to 1 string
        FILE *f = fopen(currentFile, "w"); // Open new file for writing
		if (f == NULL) perror ("Error opening file.\n");
        else {
            fprintf(f, "ROOM NAME: %s\n", roomNames[i]); // each room now has a name
        }
        fclose(f);
    }

    char *newNames[7];
    for(i = 0; i < 7; i++) { // Store the chosen 7 in a new array to avoid accessing unchosen names (index 7-9)
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
    	shuffle(newNames, 7); // Shuffle array to ensure unique connections
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

// This is used for shuffling my array of potential room names to ensure unique runs
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