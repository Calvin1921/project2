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


/**
* Main program execution
*/
#define maxArgLength 256
char *argl[maxArgLength];
pid_t pid = 0;
int pipecount = 0;
int leftcount = 0;
int rightcount = 0;
int tokencount = 0;

void noPipe() {
	
	if( leftcount == 0 && rightcount == 0 ) {	
		argl[tokencount] = NULL;
		execvp(argl[0], argl);
		perror("execvp error");
		_exit(2);
	} else {
		int n = 0;
		int check = 0;
		for( n=0; n<tokencount; n++ ) {
			int in, out;
			//out
			if(check == 1) {
				out = open( argl[n], O_CREAT | O_WRONLY, 0644);
				if(out == -1) {
					perror("check1 open");
				}
				dup2(out, STDOUT_FILENO);
				check = 0;
			} else if( check == 2) {
				in = open( argl[n], O_RDWR, 0644 );
				if(in == -1) {
					perror("check2 open");
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
		argl[tokencount] = NULL;
		printf( "%c",*argl[0] );
		execvp(argl[0], argl);
		perror("execvp error");
		_exit(2);
	}
//end of method	works great
}


//-------this works for a single pipe or a pipe with a redirect for input 
//-------like < symbol before the | and no redirects after |
void onePipe() {
	pid_t pid2 = 0;
	int fd[2];
	int n = 0;
	int check = 0;
	char *argP[tokencount];
	char *argC[tokencount];
	int tempSize = 0;
	for(n=0; n<tokencount; n++ ) {
		if(check == 1) {
			if(*argl[n] =='<' || *argl[n] == '>') {
				argC[tempSize] = NULL;
			} else {
				argC[tempSize] = argl[n];
			}
			tempSize++;
		} else {
			argP[n] = argl[n];
			if(*argl[n] == '|') {
				argP[n] = '\0';
				check = 1;
			}
			if(*argl[n] == '<' || *argl[n] == '>') {
				argP[n] = NULL;
			}
		}
	}
	argC[tempSize] = '\0';
	/*
	for(n=0; n<tokencount; n++) {
		printf("argP: %s\n", argP[n]);
		
	}
	for(n=0; n<tokencount; n++) {
		printf("argC: %s\n", argC[n]);
	}*/
	
	check = 0;
	for( n=0; n<tokencount; n++ ) {
		int in, out;
		//out
		if(check == 1) {
			out = open( argl[n], O_CREAT | O_WRONLY, 0644);
			if(out == -1) {
				perror("check1 open");
			}
			dup2(out, STDOUT_FILENO);
			check = 0;
		} else if( check == 2) {
			in = open( argl[n], O_RDWR, 0644 );
			if(in == -1) {
				perror("check2 open");
			}
			dup2(in, STDIN_FILENO);
			check = 0;
		} else if( check == 3 ) {
			/*attempt to catch redirect symbols after pipe in input chars
			int nc = 0;
			int checkTwo = 0;
			for(nc=0; nc<tokencount; nc++) {
				int out = 0;
				if(checkTwo == 1) {
					out = open( argC[n], O_CREAT | O_WRONLY, 0644);
					if(out == -1) {
						perror("check1 open");
					}
					dup2(out, STDOUT_FILENO);
				}
				if(*argC[nc] == '>') {
					checkTwo = 1;
				}
			}*/
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
					close(fd[1]);
					//printf("made it here\n");
					execvp(argP[0], argP);
					_exit(2);
				}	
			}
		}
		if(*argl[n] == '>') {
			printf( "HEll Yea: \n" );
			argl[n] = NULL;
			check = 1;
		} else if(*argl[n] == '<') {
		//printf( "another HEll Yea: \n" );
			argl[n] = NULL;
			check = 2;
		} else if(*argl[n] == '|') {
			argl[n] = NULL;
			check = 3;
		} 
	}
	/*
	argl[tokencount] = NULL;
	printf( "%c",*argl[0] );
	execvp(argl[0], argl);
	perror("execvp error");
	_exit(2);*/	
}

int main( int argc, char *argv[] )
{
	int l;
	TOKENIZER *tokenizer;
	char string[256] = "";
	char *tok;
	int br;
	string[255] = '\0';  
	printf( "\n\nSeaShell: \n" );
	while ((br = read( STDIN_FILENO, string, 255 )) > 0) { 
		if(br <= 1)
			continue;
		string[br-1] = '\0';  
		tokenizer = init_tokenizer( string );
		tokencount = 0;
		pipecount = 0;
		leftcount = 0;
		rightcount = 0;
		int i = 0;
		while( (tok = get_next_token( tokenizer )) != NULL ) {
			//char *tempora = tok;
			tokencount += 1;
			if( tok[0] == '<' ) {
				leftcount += 1;
			} else if( tok[0] == '>' ) {
				rightcount += 1;
			} else if( tok[0] == '|' ) {
				pipecount += 1;
			} 
			argl[i] = tok;
			i = i + 1;	
			//free( tok );
			//free( tempora );
		}
		
		free_tokenizer( tokenizer );
		pid = fork();
		if(pid ==-1) {
			perror("first fork error");
		}
		if(!pid) {
			//---------------block 1----------no pipes----------------
			if( pipecount == 0 ) {
				noPipe();
			}
			//---------------block 2----------one pipe----------------
			else if( pipecount == 1 ) {
				onePipe();
				
			}
			//---------------block 3----------two pipes----or many----- 
			else {
				//multiple pipes
				
			}
		} else {
			wait(&l);
		}
		printf( "SeaShell: \n" );		
	 }

	 printf( "\nBye!\n" );
	 return 0; /* all's well that end's well */
}
