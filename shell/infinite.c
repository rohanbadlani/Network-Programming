#include<stdio.h>
#include<signal.h>
#include<stdlib.h>
void signal_handler(int signal_number){
	printf("Restarting\n\n");
	exit(1);
}
int main(){
	signal(SIGCONT, signal_handler);
	while(1){
		printf("Here\n");
		sleep(2);
	}
	printf("No execution.\n");
	return 0;
}
