#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<signal.h>
#include<string.h>	

//This is a structure for the job. Maintaining some information about running jobs.

struct job{
	//jid assigned hrough count in program
	int jid;
	char cmd[100];
	pid_t pid;
	int flag;
	//flag = 1 =>bg, flag = 0 => fg/terminated
};

typedef struct job Job;
int job_number = 0;
Job background_array[100];
char running[100] = "RUNNING";
char terminated[100] = "TERMINATED";


int parse(char *cmd, char parse_cmd[10][100]){
	//parses the cmd and returns the count of the arguments in count.
	int count = 0;
	int index = 0;
	int character = 0;
	char eof = '\0';
	while((*cmd)!='\0'){
		if(*cmd == ' ' || *cmd == '\t' || *cmd == '\n'){
			//printf("index = %d character = %d", index, character);
			parse_cmd[index][character] = '\0';
			character = 0;
			index++;
			count++;
			cmd++;
			continue;
		}
		parse_cmd[index][character] = *cmd;
		cmd++;
		character++;
		if(*cmd == '\0'){
			//Off by one bit error. Here I am trying to enter the last string in the cmd. Loop would have completed at *cmd = '\0'
			parse_cmd[index][character] = *cmd;
			count++;
		}
	
	}	
	/*
	printf("\n Printing the parsed cmd \n");
	for(index = 0; index<count; index++){
		printf("String[%d]:%s\n", index, parse_cmd[index]);
	}
	*/
	return count;
}

void signal_handler_child(int signal_number){
	//signal handler for SIGCHLD
	//printf("\n*********SIG HANDLER CALLED***************\n");
	int status = 0;
	pid_t return_pid;
	
	while((return_pid = waitpid(-1, &status, WNOHANG))!=-1 && (return_pid != 0)){
		printf("\nBG Child with PID = %d has TERMINATED. Pls enter the command. (Commands ending with & will be run as background and without it as foreground) Enter 'exit' to exit from the shell", return_pid);
        	printf("\nshell$");
		fflush(stdout);
		
	}
	return;
}	
/*
void signal_handler_child_process(int sig, siginfo_t *siginfo, void *context)
{
	//signal(SIGCHLD, signal_handler_child);
	printf ("Child With PID: %ld, UID: %ld has been TERMINATED.\n", (long)siginfo->si_pid, (long)siginfo->si_uid);
	return;
}
*/

void execute_cmd(int count, char parse_cmd[10][100], char *cmd){
	//this method is called from the shell(parent process) to create a child to run the command given by user in shell. Wits if foreground, and doesn't wait if background.
	char *parsed[count+1];
	char *environment[] = {NULL};
	int index = 0;
	for(index = 0;index<count;index++){
		parsed[index] = parse_cmd[index];
		//printf("String[%d] %s\n", index, parsed[index]);
	}
	parsed[count] = '\0';
	
	pid_t pid;
	int status;
	if((pid = fork())<0){
		perror("Can't create a child for executing the given cmd\n");
		exit(1);
	}
	else if(pid == 0){
		printf("Executing cmd..Pls wait\n");
		if(*parsed[count-1] == '&'){
			parsed[count-1] = '\0';
		}	
		execvp(parsed[0], parsed);
		perror("Bad Command\n");
		exit(0);
		//printf("Unknown Command\n");
		return;
	}
	else{
		
		if(*parsed[count-1] == '&'){
			job_number++;
			background_array[job_number-1].jid = job_number;
			background_array[job_number-1].flag = 1;
			background_array[job_number-1].pid = pid;
			int index = 0;
			while(*cmd!='\0'){
				background_array[job_number-1].cmd[index] = *cmd;
				cmd++;
				index++;
			}
			background_array[job_number-1].cmd[index] = '\0';
			return;
		}
		waitpid(pid, &status, 0);
		printf("\nFG Child with PID = %d has TERMINATED. Pls enter the command. (Commands ending with & will be run as background and without it as foreground) Enter 'exit' to exit from the shell", pid);
		return;
	}
	return;
}
	
	
	
	
void signal_handler(int signal_number){
	//This is the signal handler for SIGINT, SIGQUIT, SIGSTP
	printf("\nInterrupt was encountered. Pls enter the command. (Commands ending with & will be run as background and without it as foreground).\nEnter 'exit' to exit from the shell\nshell$");
        fflush(stdout);
	return;
}
	
int main(int argc, char *argv[], char *env[]){
	signal(SIGCHLD, signal_handler_child);
	char *cmd = (char*) malloc(sizeof(char)*1000);
	signal(SIGINT, signal_handler);
	signal(SIGQUIT, signal_handler);
	signal(SIGTSTP, signal_handler);
	printf("\n************************************Hello World to the Shell!**************************************\n");
        printf("\nPls enter the command. (Commands ending with & will be run as background and without it as foreground) Enter 'exit' to exit from the shell\n");
	char status_pid[100];
	int status;
	while(1){
		int i;
		pid_t return_pid;
		char parsed_cmd[10][100];
		printf("\nshell$");
		fgets(cmd, 1000, stdin);
		if((*cmd=='e' || *cmd=='E')  && (*(cmd+1)=='x' || *(cmd+1)=='X')  && (*(cmd+2)=='i' || *(cmd+2)=='I') && (*(cmd+3)=='t' || *(cmd+3)=='T')){
			printf("\nShell Done.\n");
			exit(0);
		}
		if(*cmd=='f' && *(cmd+1)=='g' && *(cmd+2)==' '){
			//send SIGCONT to sigchild to restart the child process as a foreground process(i.e wait for it till it terminates).
			int x = (*(cmd+3)) - '0';
			//printf("\n\nX:%d\n\n", x);
			kill(background_array[x-1].pid, SIGCONT);
			waitpid(background_array[x-1].pid, &status, 0); 
		}
		
		else if(*cmd=='b' && *(cmd+1)=='g' && *(cmd+2)==' '){
			//send SIGCONT to sigchild to restart the child process as a background process.
			int x = (*(cmd+3)) - '0';
			//printf("\n\nX:%d\n\n", x);
			kill(background_array[x-1].pid, SIGCONT);
		}
		else if(*cmd=='k' && *(cmd+1)=='i' && *(cmd+2)=='l' && *(cmd+3)=='l' && *(cmd+4)==' '){
			//send SIGCONT to sigchild to restart the child process as a background process.
			int x = (*(cmd+5)) - '0';
			//printf("\n\nX:%d\n\n", x);
			kill(background_array[x-1].pid, SIGKILL);
		}
		else if(*cmd=='j' && *(cmd+1)=='o' && *(cmd+2)=='b' && *(cmd+3)=='s'){
			int i;
			for(i=0;i<job_number;i++){
				if(background_array[i].flag == 1){
					return_pid = waitpid(background_array[i].pid, &status, WNOHANG);
					if (return_pid == -1) {
	   					/* error */
					}				
					else if (return_pid == 0) {
						/* child is still running */
						strcpy(status_pid, running);
						printf("[%d] PID=%d STATUS = %s CMD = %s\n", background_array[i].jid,  background_array[i].pid, status_pid, background_array[i].cmd);
				
					}
					else if (return_pid == background_array[i].pid) {
						/* child is finished. exit status in   status */
						background_array[i].flag = 0;
						strcpy(status_pid, terminated);
						printf("HERE\n\n");
					}
				}
			}
		}
		else{
			int count = parse(cmd, parsed_cmd);
			//printf("Count::%d\n",count);
			execute_cmd(count, parsed_cmd, cmd);
		}	
	}
	
	return 0;
}
