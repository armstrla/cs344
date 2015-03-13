#include <stdio.h> // printf, fprintf
#include <stdlib.h> // malloc, realloc, free, exit, exec family
#include <string.h> // strtok, strcmp
#include <unistd.h> // chdir, fork, exec, pid_t, close
#include <sys/wait.h> // Uses waitpid() and other macros
#include <errno.h> // Track exit status
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> // Open files

#define TOK_DELIM " \t\r\n\a" // Deliminators for string tokenization

char *readLine();
char **splitLine(char *line);
int launch(char **args);

int numArgs, isbg;

int main() {
	int exitCalled = 0; // This will control the while loop
	int exitStatus = 0; // Used to track exit status (success = 0, failure = 1)
	int i; // For-loop controller, total argument count

	do {
		printf(": ");
		char *line = NULL; // This will store the line read from getLine
		char **args; // This will store the tokenized arguments from line
		int fd; // Future file descriptor 
		numArgs = 0;
		isbg = 0;

		line = readLine();
		args = splitLine(line); // Extract each token from line, store in args[]

		// Check if running in background and set the flag
		if (!(strncmp(args[numArgs - 1], "&", 1))) {
			isbg = 1;
			args[numArgs - 1] = NULL; // Clear this token
		}

		// Check for comments or blank commands
		if(args[0] == NULL || !(strncmp(args[0], "#", 1))) {
			exitStatus = 0; 
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

		// Check for redirection in the command
		else if (numArgs == 3 && ( (strcmp(args[1], ">") == 0) || (strcmp(args[1], "<") == 0) )) {
			int savedStdout, savedStdin; // Save standard input and output to be restored after redirection
			savedStdout = dup(1);
			savedStdin = dup(0);
			if (strcmp(args[1], ">") == 0) {
				fd = open(args[2], O_WRONLY|O_CREAT|O_TRUNC, 0644); // Open the file to be directed to, which is 1 index after '>'
				if (fd == -1) {
					printf("No such file or directory\n");
					exitStatus = 1;
				} else {
					dup2(fd, 1); // Point stdout to this file

					// Now that streams are established, terminate anything after the first command by setting a NULL value
					args[1] = NULL;

					close(fd); // Close unused file descriptor;
					exitStatus = launch(args); // Attempt to launch command with arguments
				}
			} else if (strcmp(args[1], "<") == 0) {
				fd = open(args[2], O_RDONLY); // Open the file to be read from
				if (fd == -1) {
					printf("No such file or directory\n");
					exitStatus = 1;
				} else {
					dup2(fd, 0); // Point stdout to this file

					// Place the command in the first argument, and then NULL-terminate anything after
					args[1] = NULL;
					//args[2] = NULL;

					close(fd); // Close unused file descriptor;
					exitStatus = launch(args); // Attempt to launch command with arguments
				}
			}
			// Restore stdout and stdin
			dup2(savedStdout, 1);
			close(savedStdout);
			dup2(savedStdin, 0);
			close(savedStdin);
		}

		// All other non-builtins without arguments
		else {
			exitStatus = launch(args);
		}

		// Deallocate memory
		free(line);
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
		numArgs++; // Increment global variable tracking number of total arguments
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
	int status, exitStatus = 0;

	pid = fork(); // Fork the current process, creating a child process and storing it's pid for the parent

	if (pid == 0) { // If pid == 0, this is the child process
		if (execvp(args[0], args) == -1) { // Child process executes a new program using the passed arguments
			printf("Command or file not recognized\n");
			exit(1); // needs to exit this child.
		}
	} else if (pid < 0) { // If pid is less than 0, there was an error forking
		perror("smallsh");
	} else { // This is for the parent
    	// Fork executed successfully. Parent process waits for the child process specified with wpid
		do {
			if (isbg == 0) wpid = waitpid(pid, &status, WUNTRACED); // Uses macros provided with waitpid() to wait until child is exited or killed
			else if (isbg == 1) wpid = waitpid(-1, &status, WNOHANG);
		} while (!WIFEXITED(status) && !WIFSIGNALED(status)); // Continue waiting for the child to exited or killed
	}

	if (isbg == 1) printf("Sleeper PID: %d\n", pid); // Display child's process ID
	// Parent checks status from child process to set exitStatus flag (0 is good, not interrupted is good)
	if (status != 0 || WIFSIGNALED(status)) exitStatus = 1;

	return exitStatus;
}