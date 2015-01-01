#include<stdio.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<string.h>
#include<netinet/in.h>
#include<stdlib.h>
#define USERAGENT "HTMLGET 1.0"

char *get_query(char *host, char *page){
  char *query;
  char *getpage = page;
  char *tpl = "GET /%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n";
  if(getpage[0] == '/'){
    getpage = getpage + 1;
    fprintf(stderr,"Removing leading \"/\", converting %s to %s\n", page, getpage);
  }
  // -5 is to consider the %s %s %s in tpl and the ending \0
  query = (char *) malloc(strlen(host)+strlen(getpage)+strlen(USERAGENT)+strlen(tpl)-5);
  sprintf(query, tpl, getpage, host, USERAGENT);
  return query;
}

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

int main(){
	FILE *fp = fopen("webpages.txt", "r");
	if(fp == NULL){
		printf("webpages.txt not found. Please add the file.\n");
		return 0;	
	}
	char line[1000];
	char domain[1000];
	char page[1000];
		
	while(fscanf(fp, "%s\n", line) != EOF){
		printf("%s\n", line);
		char request[1000];
		char *request1;
		get_halves(line, domain, page);
		
		printf("\nFirst Half: %s", domain);
	     	printf("\nSecond Half: %s", page);
		sprintf(request, "GET %s HTTP/1.0\r\nHost: %s\r\nUser-Agent: HTMLGET 1.0\r\n\r\n", page, domain);
		printf("\n%s", request);
		request1 = get_query(domain, page);
		printf("Webpage requested: %s\n", line);
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
			//struct sockaddr_in *ipv = (struct sockaddr_in *)result->ai_addr; 
			
		
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

				
				int sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
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
								printf("*********************Connection Has Been Established at post 80.*********************\n");
								//GET REQUEST:
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
								//now it is time to receive the page
								char buf[2000];
								bzero(buf, 1000);
	     							recv(sockfd, buf, 1999, 0);
	    							printf("\n%s", buf);
	   							break;
							}
						}
				close(sockfd);
				
			}while((result = result->ai_next)!=NULL);
		}
		freeaddrinfo(result);
	}
	fclose(fp);
	
	
	return 0;
}
