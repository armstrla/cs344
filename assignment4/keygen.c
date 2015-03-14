/****************************************************************************

keygen: 

This program creates a key file of specified length. The characters in 
the file generated will be any of the 27 allowed characters, generated 
using the standard UNIX randomization methods. Do not create spaces 
every five characters, as has been historically done. Note that you 
specifically do not have to do any fancy random number generation: 
we’re not looking for cryptographically secure random number generation!

****************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void generateKey(int len);

int main(int argc, char *argv[]) {
	srand(time(NULL)); // Generate random number seed
	int len; // Length of key requested
	
	if (argc != 2) printf("Usage: %s <key-length>\n", argv[0]); // Check for key-length argument
	else {
		len = atoi(argv[1]); // Do a type cast here
		generateKey(len);
	}

	return 0;
}

void generateKey(int len){
	int i;
	char curChar, key[len+1];

	for (i = 0; i < len; i++){
		curChar = "ABCDEFGHIJKLMNOPQRSTUVWXYZ "[random () % 27];
		key[i] = curChar;
	}
	key[len] = '\0';

	printf("%s\n", key);
}