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
char *argl[maxArgLength];
pid_t pid = 0;
int pipecount = 0;
int leftcount = 0;
int rightcount = 0;
int tokencount = 0;
char *argP[maxArgLength];
char *argC[maxArgLength];
pid_t pid2 = 0;
int fd[2];
int inputIndex = 0;
int outputIndex = 0;
int pipeIndex = 0;
int l, ll;
int ampcount = 0;
pid_t chpid;
pid_t testpid;
pid_t testpid2;
pid_t testpid3;
pid_t terminalpid;
int fg, bg;
void handler(int);
/*
struct sigaction act;
act.sa_handler = SIG_IGN;
sigaction(SIGINT, &act, NULL);
*/

void handler(int sig) {
	//int ppid;
	if(sig == SIGCHLD) {
		pid = waitpid(-1, &l, WNOHANG | WSTOPPED);
		//printf("got SIGCHILD     %d\n", pid);
		//kill(getpid(), SIGCHLD);
		//return;
		if(WIFEXITED(l)){
			printf("Exited\n");
		}
		//killpg(pid, SIGTSTP);
		//signal(SIGTSTP, SIG_IGN);
	}
	//if(sig == SIGCHLD ) {
		//waitpid(pid, NULL, 0);
	//}
	
}
void noPipe() {
	pid = fork();
	if(pid == -1 ) {
		perror("no pipe fork 1 error");
		return;
	} else if(pid == 0) {
			//child
			int n = 0;
			int check = 0;
			//look for redirects
			for( n=0; n<tokencount; n++ ) {
				int in, out;
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
						perror("input redirect failed");
						return;
					}
					dup2(in, STDIN_FILENO);
					check = 0;
				}
				if(*argl[n] == '>') {
					//printf( "HEll Yea: \n" );
					argl[n] = NULL;
					check = 1;
				} else if(*argl[n] == '<') {
				//printf( "another HEll Yea: \n" );
					argl[n] = NULL;
					check = 2;
				} 
			}
			//for execvp to work
			argl[tokencount] = '\0';
			//sets process to new group id different from terminals
			setpgid(pid, pid);
			//this makes cat write to terminal if foreground process
			if( ampcount == 0) {
				testpid = getpgid( getpid() );
				tcsetpgrp(STDIN_FILENO, testpid);
			} else {
				//tcsetpgrp(STDIN_FILENO, terminalpid);
			}
			execvp(argl[0], argl);
			perror("execvp error");
			_exit(2);		
	} else {
		//parent
		// set parent group id
		setpgid(getpid(), getpid());
		//wait if foreground process
		if( ampcount == 0) {
			waitpid(pid, &l, WUNTRACED);
			//give parent foreground control
			testpid2 = getpgid(getpid());
			tcsetpgrp(STDIN_FILENO, testpid2);
		} else {
			//signal(SIGCHLD, handler);
			printf("[1] %d\n", pid);
			//tcsetpgrp(STDIN_FILENO, terminalpid);
			printf("Running: %d \n",pid);
			//int a;
			/*for(a=0;a<tokencount; a++){
				printf("%s ", argl[a]);
			}
			printf( "\n" );*/
			
		}
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
	
	for(n=0; n<tokencount; n++ ) {
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
		} else {
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
	printf("this is input index: %d\n", inputIndex);
	printf("this is output index: %d\n", outputIndex);
	printf("this is pipe index: %d\n", pipeIndex);
	for(n=0; n<tokencount; n++) {
		printf("argP: %s\n", argP[n]);
		
	}
	for(n=0; n<tokencount; n++) {
		printf("argC: %s\n", argC[n]);
	}	
}

int main( int argc, char *argv[] )
{
	/*struct sigaction act;
	act.sa_handler = SIG_IGN;
	sigaction(SIGINT, &act, NULL);*/
	TOKENIZER *tokenizer;
	char string[256] = "";
	char *tok;
	int br;
	string[255] = '\0';  
	
	terminalpid = getpgid(getpid());
	//printf("this is shells pid: %d\n", terminalpid);
	signal(SIGTTOU, SIG_IGN); //*****need this to run cat********
	signal(SIGTSTP, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	//signal(SIGCHLD,handler);
	printf( "\nSeaShell: \n" );
	while ((br = read( STDIN_FILENO, string, 255 )) > 0) { 
		
		if(br <= 1) {
			printf( "SeaShell: \n" );
			continue;
		}
		string[br-1] = '\0';  
		tokenizer = init_tokenizer( string );
		tokencount = 0;
		pipecount = 0;
		leftcount = 0;
		rightcount = 0;
		ampcount = 0;
		fg=0;
		bg=0;
		int i = 0;
		while( (tok = get_next_token( tokenizer )) != NULL ) {
			tokencount += 1;
			if( tok[0] == '<' ) {
				leftcount += 1;
			} else if( tok[0] == '>' ) {
				rightcount += 1;
			} else if( tok[0] == '|' ) {
				pipecount += 1;
			} else if(tok[0]=='f' && tok[0] =='g'){
				fg=1;
				
			} else if(tok[0]=='b'&& tok[0] =='g'){
				bg=1;
			}
			argl[i] = tok;
			i = i + 1;	
			//free( tok );
			//free( tempora );
		}
		if( *argl[i-1] == '&' ) {
				ampcount += 1;
				argl[i-1] = NULL;
				tokencount = tokencount -1;
		}
		free_tokenizer( tokenizer );
		if(leftcount > 1 || rightcount > 1 || pipecount > 1 || ampcount > 1) {
			printf("wrong commands dammit!\n");
			return 0;
		}
		
		
		//---------------block 1----------no pipes----------------
		if( pipecount == 0 ) {
			//signal(SIGCHLD,handler);
			noPipe();
			if(ampcount>0){
				signal(SIGCHLD,handler);
			}
		}
		//---------------block 2----------one pipe----------------
		else if( pipecount == 1 ) {
			onePipe();
			if(pipe(fd) == -1) {
				perror("pipe error");
				continue;
			}
			pid = fork();
			if(pid == -1) {
				perror("pipe1 error in fork1");
				continue;
			} else if(pid == 0 ) {
				
				if( leftcount == 1	) {
					int in;
					in = open( argl[inputIndex], O_RDWR, 0644 );
					if(in == -1) {
						perror("input error before pipe");
						continue;
					} 
					dup2(in, STDIN_FILENO);
					close(in);
				}
				setpgid(pid, pid);
				if( ampcount == 1 ) {
					testpid = getpgid( getpid() );
					tcsetpgrp(STDIN_FILENO, testpid);
				}
				dup2(fd[1], 1);
				close(fd[1]);
				close(fd[0]);		
				execvp(argP[0], argP);
				_exit(2);
			} else {
				setpgid(pid, pid);
				pid2 = fork();
				if(pid2 == -1) {
					perror("onepipe fork2 error");
					continue;
				} else if(pid2 == 0) {
					
					if( rightcount == 1) {
						int out;
						out = open( argl[outputIndex], O_CREAT | O_WRONLY, 0644);
						if(out == -1) {
							perror("output error after pipe");
							continue;
						}
						dup2(out, STDOUT_FILENO);
						close(out);
					} 
					dup2(fd[0], 0);
					close(fd[1]);
					close(fd[0]);	
					execvp(argC[0], argC);
					_exit(2);
				} else {
					setpgid(pid2, pid);
					if( ampcount == 0 ) {
						close(fd[1]);
						close(fd[0]);
						waitpid(pid, &l, WUNTRACED);
						waitpid(pid2, &l, WUNTRACED);
						testpid2 = getpgid(getpid());
						tcsetpgrp(STDIN_FILENO, testpid2);
						
					} else {
	
					}
					
					printf( "\nSeaShell: \n" );					
				}
			}
			
			
		}
		//--------------clear arrays-------------------- 
		int nnn = 0;
		for(nnn = 0; nnn<tokencount; nnn++) {
			argC[nnn] = NULL;
			argP[nnn] = NULL;
		}
		printf( "SeaShell: \n" );		
	 }
	//printf( "\n\nSeaShell: \n" );
	printf( "\nBye!\n" );
	return 0; /* all's well that end's well */
}


/*
		while((chpid = ) > 0) {
			
			if(WIFSIGNALED(l)) {
				printf("terminated with signal %d\n", WTERMSIG(l));
			} else if(WIFEXITED(l)) {
				printf("process exited with status %d\n", WEXITSTATUS(l));
			}
		}*/
