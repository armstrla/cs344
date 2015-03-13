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

int numArgs;

int main() {
	int exitCalled = 0; // This will control the while loop
	int exitStatus = 0; // Used to track exit status (success = 0, failure = 1)
	int i; // For-loop controller, total argument count

	do {
		printf(": ");
		char *line = NULL; // This will store the line read from getLine
		char **args; // This will store the tokenized arguments from line
		//int fd = 0, fd2 = 0, fd3 = 0, fd4 = 0;
		int fd, fd2, fd3, fd4;
		numArgs = 0;

		line = readLine();
		args = splitLine(line); // Extract each token from line, store in args[]
		// printf("There were %d arguments in that command\n", numArgs); // Debug

		// Comments and blank commands
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
		else if (numArgs == 3) {
			int savedStdout; // Save standard output to be restored after redirection
			savedStdout = dup(1);
			if (strcmp(args[1], ">") == 0) {
				fd = open(args[2], O_WRONLY|O_CREAT|O_TRUNC, 0644); // Open the file to be directed to, which is 1 index after '>'
				if (fd == -1) {
					perror("open\n");
					exitStatus = 1;
				}
				dup2(fd, 1); // Point stdout to this file

				// Now that streams are established, terminate anything after the first command by setting a NULL value
				args[1] = NULL;

				close(fd); // Close unused file descriptor;
				exitStatus = launch(args); // Attempt to launch command with arguments
			} else if (strcmp(args[1], "<") == 0) {
				fd = open(args[0], O_WRONLY|O_CREAT|O_TRUNC, 0644); // Open the file to be directed to, which is 1 index after '>'
				if (fd == -1) {
					perror("open\n");
					exitStatus = 1;
				}
				dup2(fd, 1); // Point stdout to this file

				// Place the command in the first argument, and then NULL-terminate anything after
				args[0] = args[2];
				args[1] = NULL;

				close(fd); // Close unused file descriptor;
				exitStatus = launch(args); // Attempt to launch command with arguments
			}
			// Restore stdout
			dup2(savedStdout, 1);
			close(savedStdout);
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
			printf("Command not recognized\n");
			exit(1); // needs to exit this child.
		}
	} else if (pid < 0) { // If pid is less than 0, there was an error forking
		perror("smallsh");
	} else { // This is for the parent
    	// Fork executed successfully. Parent process waits for the child process specified with wpid
		do {
			wpid = waitpid(pid, &status, WUNTRACED); // Uses macros provided with waitpid() to wait until child is exited or killed
		} while (!WIFEXITED(status) && !WIFSIGNALED(status)); // Continue waiting for the child to exited or killed
	}

	// Check status from child process to set exitStatus flag (0 is good, not interrupted is good)
	if (status != 0 || WIFSIGNALED(status)) exitStatus = 1;
	//printf("217\n");

	return exitStatus;
}