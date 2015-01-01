#include<stdio.h>
#include<unistd.h>
#include<stdlib.h>
#include<string.h>
#include<sys/types.h>

void clear_buffer(char string[], int length){
	int i;
	for(i=0;i<length;i++){
		string[i] = ' ';
	}
	return;
}

int main(int argc, char *argv[]){
	int args = argc - 1;
	//printf("ARGS:: %d\n", args);
	char msg[100];
	char space[2];
	char buf[1000];
	space[0] = ' ';
	space[1] = '\0';
	int i;
	int j;
	
	if(argc == 1){
		printf("NO ARGUMENTS. RUNNING INSTRUCTIONS: ./problem passing_text\n");
		exit(0);
	}
	//printf("Msg at beg %s\n", msg);
	for(j=1;j<=args;j++){
		strcat(msg, argv[j]);
		if(j!=args){
			strcat(msg, space);
		}
		
	}
	int length = strlen(msg);
	if(length < 5){
		printf("Message should have at least 5 charactes.\n");	
		exit(0);
	}
	//printf("\n\nMsg recd:: %s\n\n", msg);
	pid_t chld1, chld2, chld3, chld4, chld5;
	
	int pipe1[2];
	int bytes_read = 0;
	pipe(pipe1);
	int pipe2[2];
	pipe(pipe2);
	int pipe3[2];
	pipe(pipe3);
	int pipe4[2];
	pipe(pipe4);
	int pipe5[2];
	pipe(pipe5);
	int pipe6[2];
	pipe(pipe6);
	if((chld1 = fork())==0){
		close(pipe1[1]);
		close(pipe2[0]);
		close(pipe3[0]);
		close(pipe3[1]);
		close(pipe4[0]);
		close(pipe4[1]);
		close(pipe5[0]);
		close(pipe5[1]);
		close(pipe6[0]);
		close(pipe6[1]);
		//bytes_read = read(pipe1[0], buf, strlen(msg));
		while(read(pipe1[0], buf, sizeof(buf))!=0);
		//printf("Child1 recd the msg -- %s\n", buf);
		for(i=0;i<strlen(buf);i++){
			if(islower(buf[i])){
				buf[i] = toupper(buf[i]);
			}
		}
		printf("C1 %d %s\n",getpid(),buf);
		write(pipe2[1], buf, sizeof(buf));
		close(pipe2[1]);
		return 0;
	}
	else if(chld1 < 0){
		perror("Child 1 could not be created\n");
	}
	else if((chld2 = fork()) == 0){
		close(pipe2[1]);
		close(pipe3[0]);
		close(pipe1[0]);
		close(pipe1[1]);
		close(pipe4[0]);
		close(pipe4[1]);
		close(pipe5[0]);
		close(pipe5[1]);
		close(pipe6[0]);
		close(pipe6[1]);
		//bytes_read = read(pipe2[0], buf, strlen(msg));
		while(read(pipe2[0], buf, sizeof(buf))!=0);
		int len = strlen(buf);
		//printf("Child2 recd the msg -- %s\nLength:: %d\n", buf, len);
		char buffer2[100];
		for(i=0;i<len-1;i++){
			buffer2[i] = buf[i+1];
		}
		buffer2[len-1] = '\0';
		
		printf("C2 %d %s\n",getpid(),buffer2);
		write(pipe3[1], buffer2, sizeof(buffer2));
		close(pipe3[1]);
		return 0;
	}
	else if((chld3 = fork()) == 0){
		close(pipe3[1]);
		close(pipe4[0]);
		close(pipe1[0]);
		close(pipe1[1]);
		close(pipe2[0]);
		close(pipe2[1]);
		close(pipe5[0]);
		close(pipe5[1]);
		close(pipe6[0]);
		close(pipe6[1]);
		//bytes_read = read(pipe3[0], buf, strlen(msg));
		while(read(pipe3[0], buf, sizeof(buf))!=0);
		int len = strlen(buf);
		//printf("Child3 recd the msg -- %s\nLength:: %d\n", buf, len);
		char buffer2[100];
		int i;
		for(i=0;i<len-1;i++){
			buffer2[i] = buf[i];
		}
		strncpy(buffer2,buf,strlen(buf)-1);
		buffer2[len-1] = '\0';
		printf("C3 %d %s\n",getpid(),buffer2);
		write(pipe4[1], buffer2, sizeof(buffer2));
		close(pipe4[1]);
		return 0;
	}
	else if((chld4 = fork()) == 0){
		close(pipe4[1]);
		close(pipe5[0]);
		close(pipe1[0]);
		close(pipe1[1]);
		close(pipe2[0]);
		close(pipe2[1]);
		close(pipe3[0]);
		close(pipe3[1]);
		close(pipe6[0]);
		close(pipe6[1]);
		//bytes_read = read(pipe4[0], buf, strlen(msg));
		while(read(pipe4[0], buf, sizeof(buf))!=0);
		int len = strlen(buf);
		//printf("Child4 recd the msg -- %s\nLength:: %d\n", buf, (int) strlen(buf));
		char buffer2[100];
		for(i=1;i<len;i++){
			buffer2[i-1] = buf[i];
		}
		buffer2[len-1] = '\0';
		printf("C4 %d %s\n",getpid(),buffer2);
		write(pipe5[1], buffer2, sizeof(buffer2));
		close(pipe5[1]);
		return 0;
	}
	else if((chld5 = fork()) == 0){
		close(pipe5[1]);
		close(pipe6[0]);
		close(pipe1[0]);
		close(pipe1[1]);
		close(pipe2[0]);
		close(pipe2[1]);
		close(pipe3[0]);
		close(pipe3[1]);
		close(pipe4[0]);
		close(pipe4[1]);
		//bytes_read = read(pipe5[0], buf, strlen(msg));
		while(read(pipe5[0], buf, sizeof(buf))!=0);
		//printf("Child5 recd the msg -- %s\n", buf);
		int len = strlen(buf);
		char buffer2[100];
		int i;
		for(i=0;i<len-1;i++){
			buffer2[i] = buf[i];
		}
		strncpy(buffer2,buf,strlen(buf)-1);
		buffer2[len-1] = '\0';
		printf("C5 %d %s\n",getpid(),buffer2);
		write(pipe6[1], buffer2, sizeof(buffer2));
		close(pipe6[1]);
		return 0;
	}
	else{
		close(pipe1[0]);
		close(pipe6[1]);
		close(pipe2[0]);
		close(pipe2[1]);
		close(pipe3[0]);
		close(pipe3[1]);
		close(pipe4[0]);
		close(pipe4[1]);
		close(pipe5[0]);
		close(pipe5[1]);
		//printf("Parent writing to the pipe\n");
		write(pipe1[1], msg, sizeof(msg));
		close(pipe1[1]);
		//bytes_read = read(pipe6[0], buf, strlen(msg));
		while(read(pipe6[0], buf, sizeof(buf))!=0);
		printf("P1 %d %s\n",getpid(),buf);
	}
			
	return 0;
}
