#include <stdio.h> // printf, fprintf
#include <stdlib.h> // malloc, realloc, free, exit, exec family
#include <string.h> // strtok, strcmp
#include <unistd.h> // chdir, fork, exec, and the pid_t type
#include <sys/wait.h> // Uses waitpid() and other macros
#include <errno.h> // Track exit status

#define TOK_DELIM " \t\r\n\a" // Deliminators for string tokenization

char *readLine();
char **splitLine(char *line);
int launch(char **args);

int main() {
	int exitCalled = 0; // This will control the while loop
	int exitStatus = 0; // Used to track exit status (success = 0, failure = 1)

	do {
		printf(": ");
		char *line = NULL; // This will store the line read from getLine
		char **args; // This will store the tokenized arguments from line

		line = readLine();
		args = splitLine(line); // Extract each token from line, store in args[]

		if(args[0] == NULL || !(strncmp(args[0], "#", 1))) { // If an empty command was entered
			exitStatus = 0; // signal the calling function to prompt for new input
		}
		
		// Builtin #1: cd
		else if(strcmp(args[0], "cd") == 0) {
			if(args[1]){ // Check to see if cd had an argument
				// attempt to move to that directory
				if(chdir(args[1]) != 0){
					printf("No such file or directory\n");
					exitStatus = 1;
				}
			} else { // If cd didn't have an argument, the default behavior is navigate to home dir
				printf("cd called.\n");
				chdir(getenv("HOME"));
				exitStatus = 0;
			}
		}

		// Builtin #2: exit
		else if(strcmp(args[0], "exit") == 0) {
			//printf("Exit called.\n");
			exitCalled = 1;
			exitStatus = 0;
		}

		// Builtin #3: status
		else if(strcmp(args[0], "status") == 0) {
			printf("Exit status: %d\n", exitStatus);
			exitStatus = 0;
		}

		// All other non-builtinsls

		else {
			exitStatus = launch(args);
		}

		//else {
		//	printf("Command not recognized\n");
		//	exitStatus = 1;
		//}
		

		free(line); // Deallocate memory
		free(args);
	} while (!exitCalled);
	return 0;
}

char *readLine() {
	char *line = NULL; // this will store the line read from getLine
	ssize_t bufsize = 0; // getLine will allocate an actual buffer size
	getline(&line, &bufsize, stdin);
	return line;
}

char **splitLine(char *line) {
	int bufsize = 64, position = 0; // Sets bufsize for each token equal to what's defined in the macro
	char **tokens = malloc(bufsize * sizeof(char*)); // Allocates memory for array of all tokens
	char *token;

	token = strtok(line, TOK_DELIM); // Fetch the first argument string, based on the deliminators macro
	while (token != NULL) { // Continue fetching arguments until the end is reached
		tokens[position] = token; // Store the current token in the array
		position++;

		if (position >= bufsize) { // If the end of the buffer is reached, 
			bufsize += 64; // add more space for this string token

			tokens = realloc(tokens, bufsize * sizeof(char*));
		}

		token = strtok(NULL, TOK_DELIM); // Grab the next token up until the next deliminator, and repeat
	}
	tokens[position] = NULL; // Store a null at the end of the tokens array to signify the end of the list
	return tokens;
}

int launch(char **args) {
	pid_t pid, wpid;
	int status;

	pid = fork(); // Fork the current process, creating a child process and storing it's pid for the parent
	//printf("Fork was just called...\n");
	if (pid == 0) { // If pid == 0, this is the child process
		//printf("This is the child\n");
		if (execvp(args[0], args) == -1) { // Child process executes a new program using the passed arguments
			printf("Command not recognized\n");
		} else {
			//printf("This shit happened\n");
		}
		return 1; // Return exit failure
	} else if (pid < 0) { // If pid is less than 0, there was an error forking
		perror("smallsh");
	} else {
    // Fork executed successfully. Parent process waits for the child process specified with wpid
		do {
			wpid = waitpid(pid, &status, WUNTRACED); // Uses macros provided with waitpid() to wait until child is exited or killed
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 0; // Return exit success
}