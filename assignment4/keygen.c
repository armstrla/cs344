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

int main(int argc, char *argv[]){
	int len;
	
	if (argc != 2) printf("Usage: %s <key-length>\n", argv[0]);
	else {
		len = atoi(argv[1]); // Do a type cast here
		generateKey(len);
	}

	return 0;
}

void generateKey(int len){
	printf("Len was %d\n.", len);
}