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
#define maxArgLength	256
char *argl[maxArgLength]; 
pid_t pid = 0;
int tokencount = 0;
int pipe_len = 0;


void redirect(TOKENIZER *tokenizer){
printf( "		Enter redirect method \n" );
	int i = 0;
	for( i=0; i<tokencount; i++ ) {
		printf(" ------------------------------------ i = %d\n", i);
		int in, out;
		char *temp = get_next_token( tokenizer );
				
		if(*temp == '>') {
			char *output_file = get_next_token( tokenizer );
			printf( "(>) go to check 1 \n" );
			printf(" 		output_file %s\n", output_file);
			out = open( output_file, O_CREAT | O_RDWR, 0644);
			dup2(out, STDOUT_FILENO);
			i++;
				
		} else if(*temp == '<') {
			char *input_file = get_next_token( tokenizer );
			printf( "(<) go to check 2  \n" );
			printf(" 		input_file %s\n", input_file);
			in = open( input_file, O_RDWR, 0644);
			dup2(in, STDIN_FILENO);
			i++;
		} if(*temp == '|'){
			return ;
		}else {
			argl[i] = temp;
		}
	}
	argl[tokencount] = NULL;
	free_tokenizer( tokenizer );
	printf( "Enter exit \n" );

}

void runpipe(int fd[], int cpid, char *path){
	if(cpid == 0){
		printf( "--------child--------\n" );
	 /* child */
	 	printf( "	%d\n",fd[0]);
		dup2(fd[0], STDOUT_FILENO);
		close(fd[1]);	/* the child does not need this end of the pipe */
		printf( "	%d\n",fd[0]);
		printf( "	what is path: %s \n", path );
		execvp(path, argl);
		perror(path);
	}else if(!cpid){
	printf( "--------child--------" );
		printf( "	%d\n",fd[1]);
		dup2(fd[1], STDIN_FILENO);
		close(fd[0]);	/* the parent does not need this end of the pipe */
		printf( "	%d\n",fd[1]);
		printf( "	what is path: %s \n", path );
		execvp(path, argl);
		perror(path);
	}else{
		perror("fork");
		exit(1);
	}
}






int main( int argc, char *argv[] ){
  TOKENIZER *tokenizer;
  char string[256] = "";
  char *tok;
  int br;
  int pipefd[2];

  string[255] = '\0';	  
  printf( "\n\nSeaShell: \n" );
//----------------------------------------------------------------
  while ((br = read( STDIN_FILENO, string, 255 )) > 0) {
    if (br <= 1)
      continue;
//-----------------------------------------------------------------
    string[br-1] = '\0';   
    tokenizer = init_tokenizer( string );
    tokencount = 0;
	pipe_len = 0;
//-----------------------------------------------------------------    
    while( (tok = get_next_token( tokenizer )) != NULL ) {
    	printf( "token:%s\n",tok);
    	if( *tok == '|'){
    		pipe_len +=1;
      	}
      	tokencount += 1;
      	free( tok );
    } 
printf( "How many token(tokencount): %d\n",tokencount);
printf( "How many pipe(pipe_len): %d\n",pipe_len);
    free_tokenizer( tokenizer );
    tokenizer = init_tokenizer( string );
//-----------------------------------------------------------------
    pid = fork();
	if(pid == 0 ) {
		printf( "Enter pid ==0 \n" );
		if( pipe_len ==0){
		printf( "	Enter pipe_len ==0 \n" );
			redirect(tokenizer);
			printf( "	End for loop \n" );
		}else if(pipe_len >0){
			printf( "	Enter pipe > 0 \n" );
			printf("	pipe");
			
			int i = 0;
			for( i=0; i<tokencount; i++ ) {
				printf(" ------------------------------------ i = %d\n", i);
				int out;
				char *temp = get_next_token( tokenizer );
				
				if(*temp == '>') {
					char *output_file = get_next_token( tokenizer );
					printf( "(>) go to check 1 \n" );
					printf(" 		output_file %s\n", output_file);
					out = open( output_file, O_CREAT | O_RDWR, 0644);
					dup2(out, STDOUT_FILENO);
					i++;				
				}else if(*temp == '|'){
					runpipe(pipefd, pid, get_next_token( tokenizer ));
				}else {
					argl[i] = temp;
				}
			}
			argl[tokencount] = NULL;
			free_tokenizer( tokenizer );
			printf( "	what is argl[0]: %s \n", argl[0] );
			printf( "	what is argl %s \n", *argl );
			printf( "	what is argl[0]: %s \n", argl[1] );
			printf( "	what is argl %s \n", *argl );
			execvp(argl[0], argl);
			perror("execvp error");
			
			_exit(2);
		}
		printf( "	what is argl[0]: %s \n", argl[0] );
		execvp(argl[0], argl);
		perror("execvp error");
		printf( "	exit \n" );
		_exit(2);
	}else if(!pid) {
		perror("pid fork error");	
	} else {
		int l;
		wait(&l);	
	}
	printf( "SeaShell: \n" );
  }
  
  printf( "\nBye!\n" );
  return 0;			/* all's well that end's well */
}
