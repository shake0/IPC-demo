#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define KEY 0x01
#define NUMBER_OF_SEMAPHORES 5 // A set can contain more semaphore
#define SEMAPHORE 2 // Choose a semaphore from 0 to NUMBER_OF_SEMAPHORES-1
#define SEMAPHORE_VALUE 10

// POSIX force you to write your own union semun
union semun {
	int val;
	struct semid_ds *buf;
	unsigned short *array;
	struct seminfo *__buf;
};

int getSemaphoreSet(char id);
void printSemaphoreInfo(int semaphore_id);
void removeSemaphoreSet(int semaphore_id);

void acquireResource(int semaphore_id, int amount);
void releaseResource(int semaphore_id, int amount);

int main(int argc, const char *argv[]){
	int semaphore_id = getSemaphoreSet(KEY);

	acquireResource(semaphore_id, SEMAPHORE_VALUE - 1); // Acquiring resource
	// Print semaphore parameters
	printSemaphoreInfo(semaphore_id);

	printf("Press ENTER to release lock");
	getc(stdin);
	releaseResource(semaphore_id, SEMAPHORE_VALUE -1); // Remember to release as much as you acquired

	sleep(1); // Waiting for ProcessB to terminate
	removeSemaphoreSet(semaphore_id);
	return 0;
}


int getSemaphoreSet(char id){
	/*
	* Generate a key
	* ftok is a hashing function that produce
	* a key by combining a file (current directory) and a byte (id)
	*/
	key_t semaphore_key = ftok(".", id);
	printf("Created key %X\n", semaphore_key);

	// Get semaphore set
	int semaphore_id;
	if((semaphore_id = semget(semaphore_key, NUMBER_OF_SEMAPHORES, IPC_CREAT | 0660)) == -1){
		perror("Cannot retrive semaphore set");
		exit(EXIT_FAILURE);
	}

	// Set semaphores initial value
	int i;
	unsigned short values[NUMBER_OF_SEMAPHORES];
	for(i = 0; i < NUMBER_OF_SEMAPHORES; i++)
		values[i] = SEMAPHORE_VALUE;
	union semun param;
	param.array = values;
	if(semctl(semaphore_id, i, SETALL, param) == -1){
		perror("Cannot set semaphores value");
		exit(EXIT_FAILURE);
	}

	return semaphore_id;
}

void printSemaphoreInfo(int semaphore_id){
	struct semid_ds info;

	// IPC_STAT: get descriptor structure
	// IPC_SET: set descriptor (only permissions can be changed)
	// IPC_RMID: remove semphore set
	if(semctl(semaphore_id, 0,IPC_STAT, &info) == -1){
		perror("Cannot retrieve information about the semaphore set");
		exit(EXIT_FAILURE);
	}

	printf("------------------------------\n");
	printf("SEMAPHORE SET %d\n", semaphore_id);
	printf("Permissions:\n\
	Owner   uid: %u\n\
	Owner   gid: %u\n\
	Creator uid: %u\n\
	Creator gid: %u\n\
	Mode       : %o\n", info.sem_perm.uid, info.sem_perm.gid, info.sem_perm.cuid, info.sem_perm.cgid, info.sem_perm.mode);
	printf("Time last operation        : %ld\n", info.sem_otime);
	printf("Time last change           : %ld\n", info.sem_ctime);
	printf("Number of semaphores       : %lu\n", info.sem_nsems);
	printf("------------------------------\n");

	// Get the value for each semaphore
	unsigned short buffer[NUMBER_OF_SEMAPHORES];
	union semun semaphore_info;
	semaphore_info.array = buffer;
	if(semctl(semaphore_id, 0, GETALL, semaphore_info) == -1){
		perror("Cannot retrieve information about the semaphore set");
		exit(EXIT_FAILURE);
	}

	int i;
	for(i = 0; i < info.sem_nsems; i++){
		printf("Semaphore %d: %d\n", i+1, semaphore_info.array[i]);
	}
	printf("------------------------------\n");
}

void removeSemaphoreSet(int semaphore_id){
	struct semid_ds info;

	if(semctl(semaphore_id, 0, IPC_RMID, NULL) == -1){
		perror("Cannot remove the semaphore set");
		exit(EXIT_FAILURE);
	}
	printf("Semaphore set removed\n");
}

void _setSemaphore(int semaphore_id, int amount){
	struct sembuf sem_operation;
	sem_operation.sem_num = SEMAPHORE; // The semaphore index
	sem_operation.sem_op  = amount;   // Removing amount

	// This is a blocking call
	//
	// `sem_operation.sem_op = 0` is a special value, the process will block until
	// the semaphore value is 0
	//
	// If the semaphore value is less than the amount required,
	// this process will sleep until there is an increment.
	// Otherwise IPC_NOWAIT can be set in sem_operation.sem_flg
	// and if the operation fail the EAGAIN is set.
	// 
	// This call accept a sequence of operation.
	if(semop(semaphore_id, &sem_operation, 1) == -1){
		perror("Cannot set semaphore");
		exit(EXIT_FAILURE);
	}

}

void acquireResource(int semaphore_id, int amount){
	_setSemaphore(semaphore_id, -amount);
}

void releaseResource(int semaphore_id, int amount){
	_setSemaphore(semaphore_id, amount);
}
