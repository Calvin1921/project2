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

int main( int argc, char *argv[] ){
  TOKENIZER *tokenizer;
  char string[256] = "";
  char *tok;
  int br;

  string[255] = '\0';	  
  printf( "\n\nSeaShell: \n" );
//----------------------------------------------------------------
  while ((br = read( STDIN_FILENO, string, 255 )) > 0) {
    if (br <= 1)
      continue;
//-----------------------------------------------------------------
    string[br-1] = '\0';   
    tokenizer = init_tokenizer( string );
    int tokencount = 0;
//-----------------------------------------------------------------    
    while( (tok = get_next_token( tokenizer )) != NULL ) {
    printf( "token:%s\n",tok);
      tokencount += 1;
      free( tok );
    } 
printf( "How many token(tokencount): %d\n",tokencount);
    free_tokenizer( tokenizer );
    tokenizer = init_tokenizer( string );
//-----------------------------------------------------------------
    pid = fork();
	if(pid == 0 ) {
		int i = 0;
		int check =0;
		
		for( i=0; i<tokencount; i++ ) {
		printf(" ------------------------------------ i = %d\n", i);
			int in, out;
			char *temp = get_next_token( tokenizer );
			//------------------------out
			if(check == 1) {
				printf("	check == 1\n");
				printf(" 		Temp %s\n", temp);
				out = open( temp, O_CREAT | O_RDWR, 0644);
				if(out == -1) {
					perror("check1 open");
				}
				dup2(out, STDOUT_FILENO);
				check = 0;
				
				//printf( "it hit check = 1 \n" ); 
			//------------------------in
			} else if( check == 2) {
				printf("	check == 2");
				printf(" 		Temp %s\n", temp);
				in = open( temp, O_RDWR| O_CREAT, 0644);
				if(in == -1) {
					perror("check2 open");
				}
				dup2(in, STDIN_FILENO);
				check = 0;
			}
			if(*temp == '>') {
				printf( "(>) go to check 1 \n" );
				check = 1;
			} else if(*temp == '<') {
				printf( "(<) go to check 2  \n" );
				check = 2;
			} else {
				argl[i] = temp;
			}
		}
		printf( "End for loop \n" );
		
		argl[tokencount] = NULL;
		free_tokenizer( tokenizer );
		printf( "what is argl[0]: %s \n", argl[0] );
		execvp(argl[0], argl);
		perror("execvp error");
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
