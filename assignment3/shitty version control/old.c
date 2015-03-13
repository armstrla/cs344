#include <sys/wait.h>	// Uses waitpid() and other macros
#include <unistd.h> 	// Uses chdir, fork, exec, and the pid_t type
#include <stdlib.h> 	// Uses malloc, realloc, free, exit, execvp, EXIT_SUCCESS and EXIT_FAILURE
#include <stdio.h> 		// Uses fprint, printf, stderr, getchar, perror
#include <string.h> 	// Uses strcmp and strtok
#include <errno.h> 		// Track exit status

#define READLINE_BUFSIZE 1024	// Defines the initial size allocated for a command string
#define TOK_BUFSIZE 64			// Defines the initial size allocated for a single token
#define TOK_DELIM " \t\r\n\a" 	// Defines token deliminators (spaces, tabs, newlines, etc.) for parsing commands

void cmdLoop(void);
char *smallshReadLine(void);
char **smallshSplitLine(char *line);
int smallshExecute(char **args, int status);
int smallshLaunch(char **args);
// Declarations for the built-ins
int smallshChangeDir(char **args, int status);
int smallshExit(char **args, int status);
int smallshStatus(char **args, int status);

// The following 2 declarations are done to facilitate adding more built-ins later, instead of using a static switch-statement
// List of built-in commands--for smallsh I only need these three
char *builtinStr[] = {
  "cd",
  "exit",
  "status"
};

// The functions corresponding to the built-in command list (an array of function pointers)
int (*builtinFunc[]) (char **, int) = {
  &smallshChangeDir,
  &smallshExit,
  &smallshStatus
};

// Return the number of builtins in the shell
int numBuiltins() {
  return sizeof(builtinStr) / sizeof(char *); // Returns the size of the builtins list divided by bytes for a char pointer
}

int main(int argc, char **argv) {
  	// Run command loop.
	cmdLoop();

	return EXIT_SUCCESS;
}

void cmdLoop(void) {
	char *line;
	char **args;
	int status;

	do {
		printf(": ");
		line = smallshReadLine(); // Save the entire command as a string

		args = smallshSplitLine(line); // Parse the command for individual arguments
		status = smallshExecute(args, status); // Attempt to execute the command, passing in its arguments

		free(line); // Deallocate memory when finished
		free(args); // Deallocate memory when finished
	} while (status);
}

char *smallshReadLine(void) {
	char *line = NULL; // this will store the line read from getLine
	ssize_t bufsize = 0; // getLine will allocate an actual buffer size
	getline(&line, &bufsize, stdin);
	return line;
}

char **smallshSplitLine(char *line) {
	int bufsize = TOK_BUFSIZE, position = 0; // Sets bufsize for each token equal to what's defined in the macro
	char **tokens = malloc(bufsize * sizeof(char*)); // Allocates memory for array of all tokens
	char *token;
	char **backupPointer;

	if (!tokens) { // If malloc failed, exit with error
		fprintf(stderr, "smallsh: allocation error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, TOK_DELIM); // Fetch the first argument string, based on the deliminators macro
	while (token != NULL) { // Continue fetching arguments until the end is reached
		tokens[position] = token; // Store the current token in the array
		position++;

		if (position >= bufsize) { // If the end of the buffer is reached, 
			bufsize += TOK_BUFSIZE; // add more space for this string token
			backupPointer = tokens; // Create a backup pointer to deallocate this space should something fail
			tokens = realloc(tokens, bufsize * sizeof(char*));
			if (!tokens) { // If realloc returns false, the allocation of memory failed and the program cannot continue
				fprintf(stderr, "smallsh: allocation error\n");
				free(backupPointer); // Deallocate this memory
				exit(EXIT_FAILURE);
			}
		}

		token = strtok(NULL, TOK_DELIM); // Grab the next token up until the next deliminator, and repeat
	}
	tokens[position] = NULL; // Store a null at the end of the tokens array to signify the end of the list
	return tokens;
}

int smallshLaunch(char **args) {
	pid_t pid, wpid;
	int status;

	pid = fork(); // Fork the current process, creating a child process and storing it's pid for the parent
	if (pid == 0) { // If pid == 0, this is the child process
		if (execvp(args[0], args) == -1) { // Child process executes a new program using the passed arguments
			perror("smallsh"); // If exec fails, print descriptive error message to stderr and exit the program
		}
		exit(EXIT_FAILURE);
	} else if (pid < 0) { // If pid is less than 0, there was an error forking
		perror("smallsh");
	} else {
    // Fork executed successfully. Parent process waits for the child process specified with wpid
		do {
			wpid = waitpid(pid, &status, WUNTRACED); // Uses macros provided with waitpid() to wait until child is exited or killed
		} while (!WIFEXITED(status) && !WIFSIGNALED(status));
	}

	return 1; // Return a 1, telling the calling function to prompt for input again
}

int smallshExecute(char **args, int status) {
	int i;

	if (args[0] == NULL) { // If an empty command was entered, 
		return 1; // signal the calling function to prompt for new input
	}

	// Compare the argument to list of builtins, and launch one if applicable
	for (i = 0; i < numBuiltins(); i++) {
		if (strcmp(args[0], builtinStr[i]) == 0) {
			return (*builtinFunc[i])(args, status);
		}
	}

	return smallshLaunch(args); // If no builtin match is found, attempt to launch
}

int smallshChangeDir(char **args, int status) {
	if (args[1] == NULL) { // Checks for second argument to cd (destination path)
		//fprintf(stderr, "smallsh: expected argument to \"cd\"\n");
		chdir(getenv("HOME"));
	} else {
		if (chdir(args[1]) != 0) { // chdir should return 0 if executed successfully
			perror("smallsh");
		}
	}
	return 1;
}

int smallshExit(char **args, int status) {
	return 0; // Signal to the calling function to not re-prompt for input (causing exit)
}

int smallshStatus(char **args, int status) {
	printf("Last exit status: %d\n", status);
	return 1;
}