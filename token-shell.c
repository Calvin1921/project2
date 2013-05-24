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


int main( int argc, char *argv[] )
{
  TOKENIZER *tokenizer;
  char string[256] = "";
  char *tok;
  int br;


  string[255] = '\0';	  
  printf( "\n\nSeaShell: \n" );
  while ((br = read( STDIN_FILENO, string, 255 )) > 0) {
    if (br <= 1)
      continue;
    string[br-1] = '\0';   
    tokenizer = init_tokenizer( string );
    int tokencount = 0;
    
    while( (tok = get_next_token( tokenizer )) != NULL ) {
      tokencount += 1;
      free( tok );
    }
    free_tokenizer( tokenizer );
    tokenizer = init_tokenizer( string );
    pid = fork();
	 if(pid == 0 ) {
		int i = 0;
		int check =0;
		for( i=0; i<tokencount; i++ ) {
			int out;
			char *temp = get_next_token( tokenizer );
			//out
			if(check == 1) {
				
				out = open( temp, O_CREAT | O_RDWR);
				if(out == -1) {
					perror("check1 open");
				}
				dup2(out, STDOUT_FILENO);
				check = 0;
				
				//printf( "it hit check = 1 \n" ); 
			}
			if(*temp == '>'|| *temp == '<') {
				printf( "HEll Yea: \n" );
				check = 1;
			}else {
				argl[i] = temp;
			}
		}
		argl[tokencount] = NULL;
		free_tokenizer( tokenizer );
		printf( "%c",*argl[0] );
		execvp(argl[0], argl);
		perror("execvp error");
		_exit(2);
	} else if(!pid) {
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
