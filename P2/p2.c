#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
int main(){
	int pipe_fd1[2], nbytes;
	int level_pipe[2];
	int status;
	pid_t chld1, chld2;
	char string1[] = "This";
	char readbuffer[100];
	printf("Text\tProcess ID\tParentProcessID\tLevel\n");
	printf("%s\t%d\t%d\t%d\n", string1, getpid(), getppid(), 0);
		
	pipe(pipe_fd1);
	pipe(level_pipe);
	if((chld1 = fork()) < 0){
		perror("Fork Error\n");
	}
	else if(chld1 == 0){
		//child 1 - Level1
		//Child has to recieve the text from pipe_fd1. Hence it closes pipe_fd1[1] - i.e The child cannot write into the pipe.
		/*
		close(pipe_fd1[1]);	
	
		//Read the signal from the file
		nbytes = read(pipe_fd1[0], readbuffer, sizeof(readbuffer));
		//if(read(pipe_fd1[0], readbuffer, sizeof(readbuffer)) > 0){
			//sleep(1);
		if(nbytes <= 0){
			sleep(1);
		}
		*/
		//Done Waiting - Previos level task complete.
		
		char string2[] = "is";
		printf("%s\t%d\t%d\t%d\n", string2, getpid(), getppid(),1);
		
		
		//Pipe for inter-level communication
		close(level_pipe[1]);
		while((nbytes = read(level_pipe[0], readbuffer, sizeof(readbuffer)))<=0){
			sleep(1);
		}

		//Pipe for level 2
		int pipe_fd2[2];
		pipe(pipe_fd2);
		pid_t chld11, chld12;
	
		//create two of its children
		
		if((chld11 = fork())< 0){
			perror("Fork Error\n");
		}
		else if(chld11 == 0){
			//child 1 - Level2
	                //Child has to recieve the text from pipe_fd2. Hence it closes pipe_fd2[1] - i.e The child cannot write into the pipe.
        	        /*
			close(pipe_fd2[1]);

                	//Read the signal from the file
            	    	nbytes = read(pipe_fd1[0], readbuffer, sizeof(readbuffer));
                	if(nbytes <= 0){
                        	sleep(0.5);
               	 	}
			*/
              		//Done Waiting - Previos level task complete.
                	char string4[] = "Prog";
                	printf("%s\t%d\t%d\t%d\n", string4, getpid(), getppid(),2);
			return 0;
		}

		if((chld12 = fork()) < 0){
                        perror("Fork Error\n");
                }

                else if(chld12 == 0){
                        //child 1 - Level2
                        //Child has to recieve the text from pipe_fd2. Hence it closes pipe_fd2[1] - i.e The child cannot write into the pipe.
                        /*
			close(pipe_fd2[1]);

                        //Read the signal from the file
                        nbytes = read(pipe_fd1[0], readbuffer, sizeof(readbuffer));
                        while(nbytes <= 0){
                                sleep(0.5);
                        }
			*/
                        //Done Waiting - Previos level task complete.
                        char string5[] = "ramming";
                        printf("%s\t%d\t%d\t%d\n", string5, getpid(), getppid(),2);
                        return 0;
                }

		return 0;
	}

	else if((chld2 = fork()) < 0){
		perror("Fork Error\n");
	}

	else if(chld2 == 0){
		//child 2 - Level1
                //Child has to recieve the text from pipe_fd1. Hence it closes pipe_fd1[1] - i.e The child cannot write into the pipe.
                //close(pipe_fd1[1]);

                //Read the signal from the file
                /*
		nbytes = read(pipe_fd1[0], readbuffer, sizeof(readbuffer));
                if(nbytes <= 0){
                        sleep(1);
                }
		*/
                //Done Waiting - Previos level task complete.
                char string3[] = "Network";
                printf("%s\t%d\t%d\t%d\n", string3, getpid(), getppid(),1);
                
		//write to level_pipe
		close(level_pipe[0]);
		
		write(level_pipe[1], string3, strlen(string3) + 1);
		
		//Pipe for level 2
                int pipe_fd2[2];
                pipe(pipe_fd2);
                pid_t chld21, chld22;

                //create two of its children

                if((chld21 = fork()) < 0){
                        perror("Fork Error\n");
                }
                else if(chld21 == 0){
                        //child 3 - Level2
                        //Child has to recieve the text from pipe_fd2. Hence it closes pipe_fd2[1] - i.e The child cannot write into the pipe.
                        /*
			close(pipe_fd2[1]);

                        //Read the signal from the file
                        nbytes = read(pipe_fd1[0], readbuffer, sizeof(readbuffer));
                        while(nbytes <= 0){
                                sleep(0.5);
                        }
			*/
                        //Done Waiting - Previos level task complete.
                        char string6[] = "lab";
                        printf("%s\t%d\t%d\t%d\n", string6, getpid(), getppid(),2);
                        return 0;
                }

                if((chld22 = fork())< 0){
                        perror("Fork Error\n");
                }

                else if(chld22 == 0){
                        //child 1 - Level2
                        //Child has to recieve the text from pipe_fd2. Hence it closes pipe_fd2[1] - i.e The child cannot write into the pipe.
                        //close(pipe_fd2[1]);

                        //Read the signal from the file
                        /*nbytes = read(pipe_fd1[0], readbuffer, sizeof(readbuffer));
                        if(nbytes <= 0){
                                sleep(0.5);
                        }*/
                        //Done Waiting - Previos level task complete.
                        char string7[] = "1";
                        printf("%s\t%d\t%d\t%d\n", string7, getpid(), getppid(),2);
                        return 0;
                }

		return 0;
	}
	else{
		sleep(1);
	}
	return 0;
}
