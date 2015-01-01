#include<stdio.h>
#include<sys/types.h>
#include<sys/ipc.h>
#include<sys/sem.h>
#include<stdlib.h>

#define key (101101)

#define key2 (101102)

int main(int argc, char *argv[]){
	struct sembuf sb;
	int n;
	if(argc!=2){
		printf("RUNNING INSTRUCTIONS: Pls enter a single argument stating the value of N\n");
		exit(0);
	}
	n = atoi(argv[1]);
	int semid;
	int semid2;
	semid = semget (key, 1, IPC_CREAT | 0666);
	semid2 = semget (key2, 1, IPC_CREAT | 0666);
	if (semid < 0 || semid2 < 0){
		printf ("Semaphore could not be created. \n");
		exit(0);
	}
	printf("Semaphore successfully created with id %d\n", semid);
	int count = semctl (semid, 0, GETVAL); 
	if(count == -1){
		printf("Could not recieve the semaphore value.\n");
		exit(0);
	}
	int set = semctl (semid, 0, SETVAL, n);
	int set2 = semctl (semid2, 0, SETVAL, 1);
	if(set == -1){
		printf("Could not set the semaphore id: %d's value.\n", semid);
	}
	printf("The semaphore value has been set to %d.\n", n);
	int parent_pid;
	parent_pid = getpid();
	pid_t chld_array[n];
	int t = n;
	struct sembuf child_acquire;
	//semid for decrementing the count of the semval
	child_acquire.sem_num = 0;
	child_acquire.sem_op = -1;
	child_acquire.sem_flg = 0;
	
	struct sembuf child_decrement;

	//semid2 for acting as a lock on the set of operations of the child so that no other child interleaves until the prev child finishes
	child_decrement.sem_num = 0;
	child_decrement.sem_op = -1;
	child_decrement.sem_flg = 0;
		
	struct sembuf child_release;
	child_release.sem_num = 0;
	child_release.sem_op = 1;
	child_release.sem_flg = 0;
	
	printf("PID \t\t\t Semval\n");
	
	while(t--){
		chld_array[t-1] = fork();	
		if(chld_array[t-1] == 0){
			semop(semid2, &child_acquire, 1);
			semop(semid, &child_decrement, 1);
			count = semctl (semid, 0, GETVAL);
			printf("Child PID = %d \t Semval = %d\n", getpid(), count);
			semop(semid2, &child_release, 1);
			return 0;
		}
		else if(chld_array[t-1] < 0){
			printf("the child %d could not be created.\n", t);
			exit(0);
		}
	}
	if(getpid()==parent_pid){
		//parent executes here.
		struct sembuf parent_wait_zero;
		parent_wait_zero.sem_num = 0;
		//parent to wait till semval==0. thus semop() to act as blocking system call until semval = 0
		parent_wait_zero.sem_op = 0;
		parent_wait_zero.sem_flg = 0;

		
		struct sembuf parent_wait_child_finish;
		parent_wait_child_finish.sem_num = 0;
		//parent to wait till last child has executed. Thus semop() to act as blocking system call until it can acquire the semid2 semaphore
		parent_wait_child_finish.sem_op = -1;
		parent_wait_child_finish.sem_flg = 0;

		semop(semid, &parent_wait_zero, 1);
		semop(semid2, &parent_wait_child_finish, 1);
		count = semctl (semid, 0, GETVAL);
		printf("Parent PID = %d has completed its operation and is exiting now. Current Semval = %d\n", getpid(), count);
		
	}		
	return 0;
}
