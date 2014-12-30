#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
void parse(char *line, char **argv);
void sigint_handler(int signal_no){
	printf("\n\nSIGINT called\n\n");
	pid_t chld=fork();
	char *arg[64];
	char cmd[20];
        if(chld==0){
              printf("\n SIGINT has been received. Pls wait for 10 seconds after entering command.\n");
              signal(SIGINT, sigint_handler);
              int number = 0;
              number = scanf("%[^\t\n]",cmd);
              while(!number){
                    sleep(1);
              }
              parse(cmd, arg);
              if (execvp(*arg, arg) < 0) { 
                    perror("Execute Error!\n");
              }
              printf("%s\n", cmd);
              return;
        }

        else if(chld<0) 
              perror("fork");
	return;
}

void  parse(char *line, char **argv)
{
     while (*line != '\0') {       /* if not the end of line ....... */ 
          while (*line == ' ' || *line == '\t' || *line == '\n')
               *line++ = '\0';     /* replace white spaces with 0    */
          *argv++ = line;          /* save the argument position     */
          while (*line != '\0' && *line != ' ' && 
                 *line != '\t' && *line != '\n') 
               line++;             /* skip the argument until ...    */
     }
     *argv = '\0';                 /* mark the end of argument list  */
}

int main(){

	pid_t chld;
	int status;


	char cmd[20];
	char *arg[64];
	int i;
	int signo;
	sigset_t  block, prevmask;
	sigset_t pending;
	char signal_char;

	sigemptyset(&block);
	for(i=0;i<=31;i++){
		sigaddset(&block,i);

	}

	if(sigprocmask(SIG_BLOCK,&block,&prevmask)==-1){
		perror("MASK ERROR");
	}
	signo=32;
	printf("\nGenerate signals\n");

	while(1)
	{
		scanf("%c",&signal_char);
		if(signal_char == 'o'){
			break;
		}
	}
	if(sigpending(&pending)<0){
		printf("\n error");
	}
	/*if(sigprocmask(SIG_UNBLOCK, &block, &prevmask)==-1){
		perror("MASK ERROR\n");
	}*/
	//signal(SIGINT, sigint_handler);
	if(sigismember(&pending,SIGINT)){

		printf("Please wait for 5 seconds. You will be prompted for an input.\n");		
		sleep(5);
		if((chld=fork())==0) {
			printf("\nSIGINT has been received. Pls wait for 10 seconds after entering command.\n");
			signal(SIGINT, sigint_handler);
			int number = 0;
			number = scanf("%[^\t\n]",cmd);
			while(!number){
				sleep(1);
			}
			parse(cmd, arg);
			if (execvp(*arg, arg) < 0) { 
			    perror("Execute Error!\n");
        	  	}
			printf("%s\n", cmd);
			return 0;
		}

		else if(chld<0) 
			perror("fork");
		

	}
	
	//wait(&status);	
	if(sigismember(&pending,SIGQUIT)){
		printf("\nSIGQUIT RECEIVED\n");
		printf("\nChild's Process ID is %d\n", chld);
	}
	if(sigprocmask(SIG_SETMASK,&prevmask,NULL)==-1){
		perror("MASK ERROR");
	}
	return 0;
}
