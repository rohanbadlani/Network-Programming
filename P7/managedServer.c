#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <alloca.h>
#include <sys/types.h>
#include <sys/socket.h>
#include<string.h>
#include<ctype.h>
#include<sys/wait.h>
#include<errno.h>

int ChildProcess(int sock){
	//printf("Socket for child: %d\n", sock);
        struct iovec vector;
        struct msghdr msg;
        struct cmsghdr *cmsg;
	
	int file_descriptor;
	
        char range[20];
	int arr[2];

        vector.iov_base = arr;
        vector.iov_len = sizeof(arr);

        cmsg = alloca(sizeof(struct cmsghdr) + sizeof(file_descriptor));
        cmsg->cmsg_len = sizeof(struct cmsghdr) + sizeof(file_descriptor);
        //cmsg->cmsg_level = SOL_SOCKET;
        //cmsg->cmsg_type = SCM_RIGHTS;
        //memcpy(CMSG_DATA(cmsg), &file_descriptor, sizeof(file_descriptor));

        msg.msg_name = NULL;
        msg.msg_namelen = 0;
        msg.msg_iov = &vector;
        msg.msg_iovlen = 1;
        msg.msg_control = cmsg;
        msg.msg_controllen = cmsg->cmsg_len;
	
	if(!recvmsg(sock, &msg, 0)){
		printf("Could not receive the message for PID %d.\n", getpid());
		perror("Recieve Error: ");
	}
	memcpy(&file_descriptor, CMSG_DATA(cmsg), sizeof(file_descriptor));
	//printf("Got file descriptor for Child as %d & Start value = %d End value = %d\n", file_descriptor, arr[0], arr[1]);
	int n;
        int characters = 0;
	char buf[1];
	lseek(file_descriptor, 0, SEEK_SET);
	lseek(file_descriptor, arr[0], SEEK_SET);
        while((n = read(file_descriptor, buf, sizeof(buf)))>0 && characters<arr[1]){
 	       printf("%d : %c\n", getpid(), toupper(buf[0]));
               characters++;
        }
        //printf("Number of characters in bytes = %d\n",characters);
	lseek(file_descriptor, 0, SEEK_SET);
	return 0;
}

int ParentProcess(int file_descriptor, int sock, int start_value, int offset){
	//printf("File descriptor for parent: %d, Socket for parent: %d\n", file_descriptor, sock);
	struct iovec vector;
	struct msghdr msg;
	struct cmsghdr *cmsg;
	
	int arr[2];
	//arr represents the normal data to be sent to the child. arr[0] = Start value. arr[1] = Number of characters to be read.
	arr[0] = start_value;
	arr[1] = offset;
	
	vector.iov_base = arr;
	vector.iov_len = sizeof(arr);

	cmsg = alloca(sizeof(struct cmsghdr) + sizeof(file_descriptor));
	cmsg->cmsg_len = sizeof(struct cmsghdr) + sizeof(file_descriptor);
	cmsg->cmsg_level = SOL_SOCKET;
	cmsg->cmsg_type = SCM_RIGHTS;
	memcpy(CMSG_DATA(cmsg), &file_descriptor, sizeof(file_descriptor));

	msg.msg_name = NULL;
	msg.msg_namelen = 0;
	msg.msg_iov = &vector;
	msg.msg_iovlen = 1;
	msg.msg_control = cmsg;
	msg.msg_controllen = cmsg->cmsg_len;

	//printf("\n\nRange: %s", range);
	
	if(sendmsg(sock, &msg, 0) != vector.iov_len){
		perror("Parent Process: Could not send the message properly. ");
	}
	return 0;
}
	
	

int main(int argc, char *argv[]){
	char *filename;
	if(argc == 2){
		int status;
		filename = argv[1];
		//printf("\n%s\n", filename);
		int file_descriptor;
		if((file_descriptor = open(filename, O_RDONLY))<0){
			perror("Sorry, Could not open the file.\n");
		}
		else{
			//printf("FILE DESCP: %d\n", file_descriptor);
			char buf[1];
			int n;
			int characters = 0;
			while((n = read(file_descriptor, buf, sizeof(buf)))>0){
				//printf("PID = %d, Char read. %c\n", getpid(), buf[0]);
				characters++;
			}
			//printf("Number of characters in bytes = %d\n",characters);
			int start_value[4];
			int offset;
			if(characters%4 == 0){
				offset = characters/4;
			}
			else{
				offset = characters/4 + 1;
			}
			int i;
			for(i=0;i<4;i++){
				start_value[i] = i*offset;
				//printf("\nStart: %d Offset: %d\n", start_value[i], offset);
			}
			
			int child = 0;
			int socks[4][2];
			for(child = 0;child<4;child++){
				if(socketpair(PF_UNIX, SOCK_STREAM, 0, socks[child])){
					perror("Socket Error.\n");
				}
			}
			pid_t pid[4];
			for(child = 0;child<4;child++){
				if((pid[child] = fork())==0){
					//child code
					close(socks[child][1]);
					return ChildProcess(socks[child][0]);
				}
				else if(pid[child] <0){
					perror("Child could not be created.\n");
				}
				/*
				else{
					//parent code.
					close(socks[0][0]);
					ParentProcess(file_descriptor, socks[0][1], 0, 6);
				}*/
			}
			//parent code
			for(child = 0;child<4;child++){
				close(socks[child][0]);
				ParentProcess(file_descriptor, socks[child][1], start_value[child], offset);	
			}
			close(file_descriptor);
			int errno; 
			while (waitpid(-1, NULL, 0)) {
 				if (errno == ECHILD) {
      					break;
   				}
			}
			
		}
		/*
		FILE *fp;
		fp = fopen(filename, "r");
		if(fp == NULL){
			printf("Cannot open file. Pls enter the correct file.\n");
		}
		else{
			printf("Exists.\n");
		}
		*/
	}
	else{
		printf("\nRunning Instruction: ./a.out <filename>\n");
	}
	return 0;
}
