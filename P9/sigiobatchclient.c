//Website HOST CONSIDERED: www.discovery.com
#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<netinet/in.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <unistd.h>
#include <strings.h>
#include<string.h>
#include <signal.h>
#include <sys/select.h>
#include <sys/fcntl.h>
#include<errno.h>
#define F_SETSIG 10

#define USERAGENT "HTMLGET 1.0"
#define _GNU_SOURCE

#define MAXLINE 1000
int count = 0;
char buf[1000];
int sockfd;
void get_halves(char line[], char domain[], char page[]){
	int i;
	for (i = 0; i < strlen(line); i++){
		domain[i] = line[i];
		if (line[i] == '/'){
			domain[i] = '\0';
	        	break;
		}    
    	}
	int j = 0;
	for (;i < strlen(line);j++, i++){
		page[j] = line[i];
	}
	page[j] = '\0';
	return;
}

static void sigioSignalHandler (int sig, siginfo_t * si, void *ucontext){
  printf ("Signal no:%d, for fd:%d, Event code:%d,  Event band:%ld\n",si->si_signo, (int) si->si_fd, (int) si->si_code, (long) si->si_band);
  fflush (stdout);
  if (si->si_code == POLL_OUT){
      printf ("POLL_OUT event occured\n");
  }
  if (si->si_code == POLL_IN){
      printf ("POLL_IN event occured\n");
      int n = read (si->si_fd, buf, MAXLINE);
      if (n == 0){
	  printf ("EOF read on socket number %d.\n", si->si_fd);
	}
      else if (n > 0){
	  count--;
	buf[n] = '\0';
	  printf ("RESPONSE RECIEVED ON SOCKET FD = %d. LENGTH OF RESPONSE: %d\nRESPONSE:\n%s \n", si->si_fd,n, buf);
	}
      else{
	  printf ("Socket %d error\n", si->si_fd);
	}
    }
  if (si->si_code == POLL_ERR){
      int err;
      int errlen = sizeof (int);
      getsockopt (si->si_fd, SOL_SOCKET, SO_ERROR, &err, &errlen);
      if (err > 0)
	printf ("error on socket %d : %s", si->si_fd, strerror (err));
    }
  	fflush (stdout);
}

int main(int argc, char *argv[]){
	char *filename;
	if(argc!=2){
		printf("RUNNING INSTRUCTIONS: ./a.out <filename>\nPlease give pages for Website HOST: www.discovery.com\n");
		exit(1);
	}
	filename = argv[1];
		
	FILE *fp = fopen(filename, "r");
	if(fp == NULL){
		printf("%s not found. Please add the file.\n", filename);
		return 0;	
	}
	struct sigaction sa, sa1;
        memset (&sa, '\0', sizeof (sa));
        memset (&sa, '\0', sizeof (sa1));
        sigemptyset (&sa.sa_mask);
        sa.sa_flags = SA_SIGINFO;
	sa.sa_sigaction = &sigioSignalHandler;	
        sigaction (SIGIO, &sa, NULL);
        sigaction (SIGRTMIN + 1, &sa, NULL);

	char line[1000];
	char domain[1000] = "www.discovery.com";
	char page[1000];
	struct addrinfo hints, *result, *save_result;
	struct in_addr addr;
	int err;
	memset(&hints, 0, sizeof(hints));
	
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_INET;
	
	err = getaddrinfo(domain, NULL, &hints, &result);
	save_result = result;
	if(err!=0){
		printf("Domain Does Not Exist.\n");
	}
	else{
		do{
			
			if(result->ai_family == AF_INET){
				char ip_address4[100];
				struct in_addr *host_address4;
				struct sockaddr_in *ipv4 = (struct sockaddr_in *) result->ai_addr;
				host_address4 = &(ipv4->sin_addr);
				inet_ntop(result->ai_family, host_address4, ip_address4, 100);
				printf("Address: %s\n", ip_address4);
				
			}
			else{
				char ip_address6[100];
				struct in6_addr *host_address6;
				struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *) result->ai_addr;
				host_address6 = &(ipv6->sin6_addr);
				inet_ntop(result->ai_family, host_address6, ip_address6, 100);
				printf("Address: %s\n", ip_address6);
			}
				
			sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
					if(sockfd == -1){
						perror("*********************Socket fd*********************");
					}
					else{
						printf("*********************Socket Created*********************\n");
						struct sockaddr_in *server_address = (struct sockaddr_in *)result->ai_addr; 
						server_address->sin_port = htons(80);
						if(connect(sockfd, (struct sockaddr *) server_address, sizeof(struct sockaddr_in))<0){
								printf("*********************Connection Could Not Be Established at port 80*********************\n");
						}
						else{
							fcntl (sockfd,F_SETOWN, getpid ());
						        int flags = fcntl (sockfd, F_GETFL);	/* Get current flags */
       							fcntl (sockfd,F_SETFL,flags | O_ASYNC | O_NONBLOCK);	//set signal driven IO
       							fcntl (sockfd, F_SETSIG, SIGRTMIN + 1);	//replace SIGIO with realtime signal
       								
							printf("*********************Connection Has Been Established at post 80.*********************\n");
									
							while(fscanf(fp, "%s\n", line) != EOF){
									printf("%s\n", line);
									char request[1000];
									//get_halves(line, domain, page);
									//domain = "csis.bits-pilani.ac.in";
									int i;
									int j;
									for (i=0,j=0;i < strlen(line);j++, i++){
										page[j] = line[i];
									}
									page[j] = '\0';
									printf("\nFirst Half: %s", domain);
								     	printf("\nSecond Half: %s", page);
									sprintf(request, "HEAD %s HTTP/1.0\r\nHost:%s\r\nConnection: Keep-Alive\r\n\r\n", page, domain);
									printf("\n%s", request);
									printf("Webpage requested: %s%s\n", domain,page);

									int tmpres;
									int sent = 0;
									while(sent < strlen(request)){
										tmpres = send(sockfd, request+sent, strlen(request)-sent, 0);
										if(tmpres == -1){
											perror("Can't send query");
								 			exit(1);
										}
										sent += tmpres;
									}
									count++;
									printf("***REQUEST SENT: Count - %d\n", count);
											
							}
							//shutdown(sockfd, SHUT_WR);
							while(count!=0){
								pause();
							}
							break;	
						}
						
						
					}
	
				
			}while((result = result->ai_next)!=NULL);
		}
		
		freeaddrinfo(result);
		fclose(fp);
		return 0;
	}
