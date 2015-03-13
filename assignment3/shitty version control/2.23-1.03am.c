#include <stdio.h> // printf, fprintf
#include <stdlib.h> // malloc, realloc, free, exit, exec family
#include <string.h> // strtok, strcmp
#include <unistd.h> // chdir, fork, exec, and the pid_t type
#include <sys/wait.h> // Uses waitpid() and other macros
#include <errno.h> // Track exit status
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

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
		int fd = 0, fd2 = 0, fd3 = 0, fd4 = 0;
		numArgs = 0;

		line = readLine();
		args = splitLine(line); // Extract each token from line, store in args[]
		// printf("There were %d arguments in that command\n", numArgs); // Debug

		if(args[0] == NULL || !(strncmp(args[0], "#", 1))) { // If an empty command or comment was entered,
			exitStatus = 0; // ignore; signal the calling function to prompt for new input
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
			if (strcmp(args[1], ">") == 0) {
				fd = open(args[2], O_WRONLY|O_CREAT|O_TRUNC, 0644); // Open the file to be directed to, which is 1 index after '>'
				//printf("%s set as output file\n", args[2]); // Debug
				if (fd == -1) {
					//printf("Output file not found\n");
					exitStatus = 1;
				}
				fd2 = dup2(fd, 1); // Point stdout to this file
				//printf("%s set as output file\n", args[2]); // Debug
				if (fd == -1) {
					//printf("Error redirecting standard output\n");
					exitStatus = 1;
				}
				// Now that streams are established, terminate anything after the first command by setting a NULL value
				args[1] = NULL;
				args[2] = NULL;
				exitStatus = launch(args); // Attempt to launch command with arguments
				close(fd);
				close(fd2);
			}
		} else if (numArgs == 4) {
			if (strcmp(args[2], ">") == 0 ) {
				fd = open(args[3], O_WRONLY|O_CREAT|O_TRUNC, 0644); // Open the file to be directed to, which is 1 index after '>'
				if (fd == -1) {
					printf("Output file not found\n");
					exitStatus = 1;
				}
				fd2 = dup2(fd, 1); // point stdout to it
				if (fd == -1) {
					printf("Error redirecting standard output\n");
					exitStatus = 1;
				}
				fd3 = open(args[1], O_RDONLY); // Open the file to be read from, which comes before '>'
				if (fd == -1) {
					printf("Output file not found\n");
					exitStatus = 1;
					exit(1);
				}
				fd4 = dup2(fd, 0); // Point stdin to it
				if (fd == -1) {
					printf("Error redirecting standard input\n");
					exitStatus = 1;
				}
				// Now that streams are established, terminate anything after the first command by setting a NULL value
				args[1] = NULL;
				args[2] = NULL;
				args[3] = NULL;
				exitStatus = launch(args); // Attempt to launch command with arguments
				close(fd);
				close(fd2);
				close(fd3);
			}
		}

		// All other non-builtins without arguments
		else {
			exitStatus = launch(args);
		}

		// Close file descriptors
		//if (fd != 0) close(fd);
		//if (fd2 != 0) close(fd2);
		//if (fd3 != 0) close(fd3);
		//if (fd4 != 0) close(fd4);

		//printf("Does it make it here?\n");

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
	//printf("Fork was just called...\n");
	if (pid == 0) { // If pid == 0, this is the child process
		//printf("This is the child\n");
		if (execvp(args[0], args) == -1) { // Child process executes a new program using the passed arguments
			printf("Command not recognized\n");
			exit(1); // needs to exit this child.
		}
	} else if (pid < 0) { // If pid is less than 0, there was an error forking
		perror("smallsh");
		// Needs to exit>
	} else { // This is for the parent
    	// Fork executed successfully. Parent process waits for the child process specified with wpid
		do {
			wpid = waitpid(pid, &status, WUNTRACED); // Uses macros provided with waitpid() to wait until child is exited or killed
		} while (!WIFEXITED(status) && !WIFSIGNALED(status)); // Continue waiting unless child is exited or killed
	}

	// Check status from child process to set exitStatus flag (0 is good, not interrupted is good)
	if (status != 0 || WIFSIGNALED(status)) exitStatus = 1;

	return exitStatus; // Return exit success
}