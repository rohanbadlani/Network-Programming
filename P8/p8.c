#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netdb.h>
#include<netinet/in.h>
#include<string.h>
#include<pthread.h>
#include<sys/time.h>
#define USERAGENT "HTMLGET 1.0"
char *server_name;
int server_port;
int N;
int req_per_con;
char *file_to_req;
float global_response_time[1000];
pthread_mutex_t write_mutex = PTHREAD_MUTEX_INITIALIZER;

void *thread_function(void *vptr){
	pthread_detach(pthread_self());
	//printf("Thread Running.\n");
	struct addrinfo hints, *result, *save_result;
	struct in_addr addr;
	int err;
	
	//time_t t1;
	//time_t t2;
	struct timeval t1;
	struct timeval t2;
	/*
	char *server_name;
	server_name = strtok(vptr," ");
	char *file_to_req;
	file_to_req = strtok(NULL," ");
	int req_per_con;
	req_per_con = atoi(strtok(NULL," "));
	printf("SERVER NAME = %s, FILE = %s, REQ = %d\n", server_name, file_to_req, req_per_con);
	*/
	long response_time = 0;
	float response_time_av = 0;
	memset(&hints, 0, sizeof(hints));
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_family = AF_INET;
	err = getaddrinfo(server_name, NULL, &hints, &result);
	save_result = result;
	
	if(err!=0){
		printf("Domain Does Not Exist.\n");
	}
	else{
		int j;	
		for(j=0;j<req_per_con;j++){
			do{
			
				if(result->ai_family == AF_INET){
					char ip_address4[100];
					struct in_addr *host_address4;
					struct sockaddr_in *ipv4 = (struct sockaddr_in *) result->ai_addr;
					host_address4 = &(ipv4->sin_addr);
					inet_ntop(result->ai_family, host_address4, ip_address4, 100);
					//printf("Address: %s\n", ip_address4);
				
				}
				else{
					char ip_address6[100];
					struct in6_addr *host_address6;
					struct sockaddr_in6 *ipv6 = (struct sockaddr_in6 *) result->ai_addr;
					host_address6 = &(ipv6->sin6_addr);
					inet_ntop(result->ai_family, host_address6, ip_address6, 100);
					//printf("Address: %s\n", ip_address6);
				}
				
				int sockfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
				if(sockfd == -1){
					perror("*********************Socket fd Error*********************");
				}
				else{
					//printf("*********************Socket Created*********************\n");
					struct sockaddr_in *server_address = (struct sockaddr_in *)result->ai_addr; 
					server_address->sin_port = htons(server_port);
					if(connect(sockfd, (struct sockaddr *) server_address, sizeof(struct sockaddr_in))<0){
						printf("*********************Connection Could Not Be Established at port 80*********************\n");
					}
					else{
						printf("Connection Has Been Established at port 80. for ThreadID = %d, Request Number = %d\n",(int) pthread_self(), j+1);
						//GET REQUEST:
						char *query;
						char *tpl = "GET /%s HTTP/1.0\r\nHost: %s\r\nUser-Agent: %s\r\n\r\n";
						query = (char *) malloc(strlen(server_name)+strlen(file_to_req)+strlen(USERAGENT)+strlen(tpl)-5);
	  					sprintf(query, tpl, file_to_req, server_name, USERAGENT);
	  
						int tmpres;
						int sent = 0;
						gettimeofday(&t1,NULL);
						while(sent < strlen(query)){
							tmpres = send(sockfd, query+sent, strlen(query)-sent, 0);
							if(tmpres == -1){
								perror("Can't send query");
					 			exit(1);
							}
							sent += tmpres;
						}
					
						//now it is time to receive the page
						char buf[1];
						bzero(buf, 1);
						recv(sockfd, buf, 1, 0);
						printf("ThreadID = %d, REQ NUMBER = %d\n", (int) pthread_self(), j+1);
						gettimeofday(&t2, NULL);
						response_time += (t2.tv_sec - t1.tv_sec)*1000000L + (t2.tv_usec - t1.tv_usec); 
						//t2 = time(NULL);
						//response_time += ((long) t2) - ((long) t1);
						//printf("Thread: %d, TB = %lld, TA = %lld\n", (int) pthread_self(),(long long) t1,(long long) t2);

						//printf("Thread: %d, Req Number = %d Res Time = %ld\n", (int) pthread_self(), j+1, response_time);
				
				
					}
				
					break;
				}
		
			close(sockfd);
				
			}while((result = result->ai_next)!=NULL);
			
			memset(&hints, 0, sizeof(hints));
			
			
		}
		freeaddrinfo(result);
		response_time_av = response_time/req_per_con;
		//WRITE MUTEX IMPLEMENTED TO MAINTAIN SHARED RESOURCE CONSISTENTCY 
		int k;
		//printf("Thread: %d, Response Time = %f\n", (int) pthread_self(), response_time_av);
		pthread_mutex_lock(&write_mutex);
		k = 0;
		while(global_response_time[k]!=0){
			k++;
			//printf("Thread ID: %d Response time = %f", (int) pthread_self(), response_time_av);
		}
		if(k == 1000){
			printf("Cannot Write.\n");
			exit(0);
		}
		global_response_time[k] = response_time_av;
		pthread_mutex_unlock(&write_mutex);
	}
	
	return (NULL);
	
}
int main(int argc, char *argv[]){
	if(argc == 6){
		memset(&global_response_time, 0, sizeof(global_response_time));
		server_name = argv[1];
		server_port = atoi(argv[2]);
		N = atoi(argv[3]);
		req_per_con = atoi(argv[4]);
		file_to_req = argv[5];
		printf("\nRequest: Server_Name = %s, Server_Port = %d, N = %d, req_per_com = %d, file_to_con = %s\n\n",server_name,server_port,N,req_per_con,file_to_req);
		char *send_to_thread;
	        char *tpl = "%s\0 %s\0 %d\0 ";
                send_to_thread = (char *) malloc(strlen(server_name)+strlen(file_to_req)+sizeof(int)+4);
                sprintf(send_to_thread, tpl, server_name, file_to_req, req_per_con);

		pthread_t thread_id[N];
		int err;
		int i;
		for(i=0;i<N;i++){
			err = pthread_create(&thread_id[i], NULL, thread_function, (void *)send_to_thread);
			if(err == 0){
				//printf("Created Thread No %d.\n",i);
				
			}
			else{
				printf("Sorry the Thread Number %d could not be created.\n", i);
			}
		}
		for(i=0;i<N;i++){
			pthread_join(thread_id[i],NULL);		
		}
		float sum = 0;
		for(i=0;i<N;i++){
			//printf("Arr[%d]::%f->",i,global_response_time[i]);	
			sum += global_response_time[i];
		}
		sum = sum/N;
		printf("\nAVG RESPONSE TIME OVER ALL %d THREADS WITH %d REQUESTS = %f MICROSECONDS(us).\n",N,req_per_con,sum);
	
	}
	else{
		printf("RUNNING INSTRUCTIONS:\n./a.out <server_name> <server_port> <N> <req_per_con> <file_to_req>\nThe value of N must be less than 1000;\n");
	}
	return 0;
}
	
