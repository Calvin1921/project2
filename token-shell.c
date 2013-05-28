#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include "tokenizer.h"
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <signal.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>


/**
* Main program execution
*/
#define maxArgLength 256
char *argl[maxArgLength]; //store all the string
pid_t pid = 0;
char *argP[maxArgLength]; //store string before '|'
char *argC[maxArgLength]; // store string after '|'
int pipecount = 0;
int leftcount = 0;
int rightcount = 0;
int tokencount = 0;
<<<<<<< HEAD
/*
 * 
 */
void redict(const char *path, int stdin, int flag){
		int inOrOut;
		//open
		inOrOut = open( path, flag, 0644 );
		if(inOrOut == -1) {
			perror("check2 open");
		}
		dup2(inOrOut, stdin);
}

/*
 * deal with redirect without pipe
 */
void noPipe() {
	int n = 0;
	int check = 0;
	//loop over the argl array
	for( n=0; n<tokencount; n++ ) {
		if(check == 1) {
			//deal with '>'
			redict(argl[n], STDOUT_FILENO,O_CREAT | O_WRONLY);
			check = 0;
		} else if( check == 2) {
			//deal with '<'
			redict(argl[n], STDIN_FILENO,O_RDWR);
			check = 0;
		}
		//check if equal to '>' or '<'
		if(*argl[n] == '>') {
			printf( "HEll Yea: \n" );
			argl[n] = NULL;
			check = 1;
		} else if(*argl[n] == '<') {
		//printf( "another HEll Yea: \n" );
			argl[n] = NULL;
			check = 2;
		} 
	}
	argl[tokencount] = NULL;
	printf( "%c",*argl[0] );
	execvp(argl[0], argl);
	perror("execvp error");
	_exit(2);
} //end of method	works great

/*
 * helper method to separte string to two part and put them into argC 
 * and argP array.
 * 
 */
void fillArg(){
	int n = 0;
	int check = 0;
	int tempSize = 0;
	//loop over the argl array
=======
char *argP[maxArgLength];
char *argC[maxArgLength];
pid_t pid2 = 0;
int fd[2];
int inputIndex = 0;
int outputIndex = 0;
int pipeIndex = 0;
int l, ll;
int ampcount = 0;

/*
struct sigaction act;
act.sa_handler = SIG_IGN;
sigaction(SIGINT, &act, NULL);
*/
void noPipe() {
	pid = fork();
	if(pid == -1 ) {
		perror("no pipe fork 1 error");
		return;
	} else if(pid == 0) {
			int n = 0;
			int check = 0;
			for( n=0; n<tokencount; n++ ) {
				int in, out;
				//out
				if(check == 1) {
					out = open( argl[n], O_CREAT | O_WRONLY, 0644);
					if(out == -1) {
						perror("output redirect failed");
						return;
					}
					dup2(out, STDOUT_FILENO);
					check = 0;
				} else if( check == 2) {
					in = open( argl[n], O_RDWR, 0644 );
					if(in == -1) {
						perror("input redirext failed");
						return;
					}
					dup2(in, STDIN_FILENO);
					check = 0;
				}
				if(*argl[n] == '>') {
					printf( "HEll Yea: \n" );
					argl[n] = NULL;
					check = 1;
				} else if(*argl[n] == '<') {
				//printf( "another HEll Yea: \n" );
					argl[n] = NULL;
					check = 2;
				} 
			}
			argl[tokencount] = '\0';
			//act.sa_handler = SIG_DFL;
			//sigaction(SIGINT, &act, NULL);
			execvp(argl[0], argl);
			perror("execvp error");
			_exit(2);		
	} else {
		wait(&l);
		printf( "\n\nSeaShell: \n" );
	}
	
//end of method	works great
}


//-------this works for a single pipe or a pipe with a redirect for input 
//-------like < symbol before the | and no redirects after |
void onePipe() {
	int n = 0;
	int check = 0;
	int tempSize = 0;
	printf("tokencount %d\n", tokencount);
>>>>>>> 317856fdd73bcb75f964459333c8831a9238d0f9
	for(n=0; n<tokencount; n++ ) {
		// check become 1 after '|'
		if(check == 1) {
			if(*argl[n] == '>') {
				outputIndex = n+1;
				argC[tempSize] = NULL;
			} else if( *argl[n] =='<' ) {
				printf("error, no input redirect allowed after pipe");
				return;
			}else {
				argC[tempSize] = argl[n];
			}
			tempSize++;
		} else { //enter before '|' and store string in to argP
			argP[n] = argl[n];
			if(*argl[n] == '|') {
				pipeIndex = n+1;
				argP[n] = '\0';
				check = 1;
			}
			if(*argl[n] == '<' ) {
				inputIndex = n+1;
				argP[n] = NULL;
			} else if( *argl[n] == '>') {
				printf("error, cannot redirect output before pipe\n");
				return;
			}
		}
	}
	argC[tempSize] = '\0';
<<<<<<< HEAD
}//end of helper method

//-------this works for a single pipe or a pipe with a redirect for input 
//-------like < symbol before the | and no redirects after |
void onePipe() {
	pid_t pid2 = 0;
	int fd[2];
	int n = 0;
	int check = 0;

	fillArg();
	
=======
	printf("this is input index: %d\n", inputIndex);
	printf("this is output index: %d\n", outputIndex);
	printf("this is pipe index: %d\n", pipeIndex);
	for(n=0; n<tokencount; n++) {
		printf("argP: %s\n", argP[n]);
		
	}
	for(n=0; n<tokencount; n++) {
		printf("argC: %s\n", argC[n]);
	}
	/*
>>>>>>> 317856fdd73bcb75f964459333c8831a9238d0f9
	check = 0;
	for( n=0; n<tokencount; n++ ) {
		if(check == 1) {
			// for redirect before pipe (create or write)
			redict(argl[n], STDOUT_FILENO,O_CREAT | O_WRONLY);
			check = 0;
		} else if( check == 2) {
			// for redirect before pipe (read or write)
			redict(argl[n], STDIN_FILENO,O_RDWR);
			check = 0;
<<<<<<< HEAD
		} else if( check == 3 ) {
			printf("hello \n");
			if(pipe(fd) == -1) {
				perror("pipe error");
			} else {
				pid2 = fork(); // fork
				
				if(pid2 == -1) { //check error
					perror("pipe1fork error");
				} else if(pid2 == 0) { /* child read from pipe*/
					dup2(fd[0], 0);  
					close(fd[1]);   /* close unused write end */
					execvp(argC[0], argC);
					_exit(2);
				} else { /* parent write to pipe*/
					//set pid2 process group id  to pid
					setpgid(getpid(), pid2);
					printf("pid2 = %d \n", getpgid(pid2));
					dup2(fd[1], 1);
					//close(fd[1]);
					if(*argl[n] == '>'){
						redict(argl[n+1], STDOUT_FILENO,O_CREAT | O_WRONLY);
						//redict(argl[n+1], STDIN_FILENO,O_RDWR);
						execvp(argP[0], argP);
					}
					//_exit(2);
				}	
			}
		}
		//check '>', '<' and '|'
=======
		} 
			
>>>>>>> 317856fdd73bcb75f964459333c8831a9238d0f9
		if(*argl[n] == '>') {
			argl[n] = NULL;
			check = 1;
		} else if(*argl[n] == '<') {
			argl[n] = NULL;
			check = 2;
		} 
<<<<<<< HEAD
	}//end of for-loop
	execvp(argP[0], argP);
	_exit(2);
}//end of method
=======
	}
	
	argl[tokencount] = NULL;
	printf( "%c",*argl[0] );
	execvp(argl[0], argl);
	perror("execvp error");
	_exit(2);*/	
}
>>>>>>> 317856fdd73bcb75f964459333c8831a9238d0f9

int main( int argc, char *argv[] )
{
	TOKENIZER *tokenizer;
	char string[256] = "";
	char *tok;
	int br;
	
	string[255] = '\0';  
	printf( "\n\nSeaShell: \n" );
	while ((br = read( STDIN_FILENO, string, 255 )) > 0) { 
		if(br <= 1) {
			printf( "SeaShell: \n" );
			continue;
		}
		string[br-1] = '\0';  
		tokenizer = init_tokenizer( string );
		tokencount = 0;
		pipecount = 0;
		//leftcount = 0;
		//rightcount = 0;
		int i = 0;
		while( (tok = get_next_token( tokenizer )) != NULL ) {
			tokencount += 1;
			//if( tok[0] == '<' ) {
				//leftcount += 1;
			//} else if( tok[0] == '>' ) {
				//rightcount += 1;
			//} else 
			if( tok[0] == '|' ) {
				pipecount += 1;
			} else if( tok[0] == '&' ) {
				ampcount += 1;
			}
			argl[i] = tok;
			i = i + 1;
		}
		free_tokenizer( tokenizer );
<<<<<<< HEAD
		pid = fork();
		printf("pid = %d \n", pid);
		if(pid ==-1) {
			perror("first fork error");
		}
		if(!pid) { /* parent*/
			setpgid(pid, pid);
			//---------------block 1----------no pipes----------------
			if( pipecount == 0 ) {
				noPipe();
			}
			//---------------block 2----------one pipe----------------
			else if( pipecount == 1 ) {
				onePipe();
				
			}
		} else { /* child */
			wait(&l);
=======
		if(leftcount > 1 || rightcount > 1 || pipecount > 1 || ampcount) {
			printf("wrong commands dammit!\n");
			return 0;
		}
		
		
		//---------------block 1----------no pipes----------------
		if( pipecount == 0 ) {
			noPipe();
		}
		//---------------block 2----------one pipe----------------
		else if( pipecount == 1 ) {
			onePipe();
			if(pipe(fd) == -1) {
				perror("pipe error");
				//continue;
			}
			pid = fork();
			if(pid == -1) {
				perror("pipe1 error in fork1");
				//continue;
			} else if(pid == 0 ) {
				if( leftcount != 0	) {
					int in;
					in = open( argl[inputIndex], O_RDWR, 0644 );
					if(in == -1) {
						perror("input error before pipe");
						//continue;
					} 
					dup2(in, STDIN_FILENO);
				}	
				dup2(fd[1], 1);
				close(fd[1]);
				close(fd[0]);
				//act.sa_handler = SIG_DFL;
				//sigaction(SIGINT, &act, NULL);
				execvp(argP[0], argP);
				_exit(2);
			} else {
				
				pid2 = fork();
				if(pid2 == -1) {
					perror("onepipe fork2 error");
					continue;
				} else if(pid2 == 0) {
					if( rightcount != 0) {
						int out;
						out = open( argl[outputIndex], O_CREAT | O_WRONLY, 0644);
						if(out == -1) {
							perror("output error after pipe");
							//continue;
						}
						dup2(out, STDOUT_FILENO);
					}
					dup2(fd[0], 0);
					close(fd[1]);
					close(fd[0]); 
					//act.sa_handler = SIG_DFL;
					//sigaction(SIGINT, &act, NULL); 
					execvp(argC[0], argC);
					_exit(2);
				} else {
					wait(&l);
					printf( "\n\nSeaShell: \n" );
					//wait(&l);
					
				}
			}
			
			
>>>>>>> 317856fdd73bcb75f964459333c8831a9238d0f9
		}
		//---------------block 3----------two pipes----or many----- 
		
				
	 }
<<<<<<< HEAD
	 printf( "\nBye!\n" );
		return 0; /* all's well that end's well */
=======
	//printf( "\n\nSeaShell: \n" );
	printf( "\nBye!\n" );
	return 0; /* all's well that end's well */
>>>>>>> 317856fdd73bcb75f964459333c8831a9238d0f9
}


/*
			int checkTwo = 0;
			int nc = 0;
			for(nc=0; nc<tokencount; nc++) {
				int out;
				if(checkTwo == 1) {
					out = open( argC[nc], O_CREAT | O_WRONLY, 0644);
					if(out == -1) {
						perror("check1 open");
					}
					dup2(out, STDOUT_FILENO);
					checkTwo = 0;
				}
				if(*argC[nc] == '>') {
					checkTwo = 1;
				}
			}
			printf("hello \n");
			if(pipe(fd) == -1) {
				perror("pipe error");
			} else {
				pid2 = fork();
				if(pid2 == -1) {
					perror("pipe1fork error");
				} else if(pid2 == 0) {
					dup2(fd[0], 0);
					close(fd[1]);  
					execvp(argC[0], argC);
					_exit(2);
				} else {
					dup2(fd[1], 1);
					close(fd[0]);
					//printf("made it here\n");
					execvp(argP[0], argP);
					_exit(2);
				}	
			}*/
