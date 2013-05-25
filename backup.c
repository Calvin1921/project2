/*
 * CIS 415 - Proj 1 - shell
 * By Jonathan Ziesing
 * Thursday April 25th, 2013
 *
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>

/*
 * define space for input 
 * create process id variable
 */
#define maxArgLength	1024

pid_t pid = 0;

/*
 *  handler handles the case of the alarm signal firing
 */

void handler(int signum) {
	
	kill(pid, SIGKILL);
}

int main(int argc, char *argv[]) {
	/*
	 *looks for time limit passed in when running program
	 */
	int timeLimit = 0;
	if(argc == 2)
	{
		timeLimit = strtol(argv[1], NULL, 10);
	}
	
	/*
	 * rawInput holds the commands from read 
 	 * argL and arg and rawinput are to pass execve
 	 */
	ssize_t numOfRead;
	char rawInput[maxArgLength];
	char* argL[] = { NULL, NULL };
	char* arg[] = { NULL };
	
	
	/*
 	 *  
 	 * infinite loop to keep shell running
 	 */

	while(1) {
		/*
 		 * write 'seaShell$', then read command, doesn't take extra param
 		 * set first of argL to input, the argument to send execve
		 * then fork
 		 */
		write(1, "SeaShell$ ", 10);
		numOfRead = read(0, rawInput, maxArgLength);
		argL[0] = rawInput;
		pid = fork();
		if(pid < 0) {
			//forking error
			perror("fork() error");
			break;
			
		} else if( pid == 0 ) {
			//sucess
			int i = 0;
			//check for new lines and eof
			while( rawInput[i] != '\n' && rawInput[i] != '\0' ) {
				i++;
			}
			//set the end of argument to execve to null for execve to work
			rawInput[i] = '\0';
			execve(rawInput, argL, arg);
			perror("execve error");
			//kills the process if an error
			_exit(2);

		} else {
			/*
	 		 * call sigalarm handler and set alarm if specified
			 * wait allows parent proccess to execute
			 * l is status of signal and if 0, it worked, if not it failed
	 		 */
			signal(SIGALRM, handler);
			if(timeLimit != 0) {
				alarm(timeLimit);
			}
			int l;
			wait(&l);
			if(l == 0 ) {
				write(1, "Congrats, you used the seaShell wisely!\n", 41);
			} else {
				write(1, "that program runs too long..\n", 29);
			}
			 

		}
	}

	return 0;
}