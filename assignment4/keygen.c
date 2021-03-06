#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

int main(int argc, char *argv[]) {
	// Check for correct args/usage
	if (argc != 2) printf("Usage: %s length\n", argv[0]); // Check for key-length argument
	else {
		srand(time(NULL)); // Generate random number seed
		int i; // Length of key requested, and for-loop controller
		int len = atoi(argv[1]); // Type cast key-length (string) to integer
		char curChar, key[len+1]; // Current character to generate, and string for all characters
		
		for (i = 0; i < len; i++){ // Loop through and generate each character randomly
			curChar = "ABCDEFGHIJKLMNOPQRSTUVWXYZ "[random () % 27];
			key[i] = curChar; // Store this random character at current index
		}
		key[len-1] = '\0'; // Null-terminate the string to avoid extra junk being printed

		printf("%s\n", key); // Output the generated string
	}

	return 0;
}