#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>

#define KEY 0x01
#define SEGMENT_SIZE 0xff // 256 bytes

int getSharedMemory(char id);
void printSharedMemoryInfo(int shared_id);
void removeSharedMemory(int shared_id);


/*
 * Shared memory do not include any lock mechanism by itself
 * synchornization should be achived by other means.
 *
 * In some extends this is very similar to mmap but the mapping
 * exists in memory only, and will not persist after reboot.
 * It's is really not a bad idea to release allocated memory once you are done
 */
int main(int argc, const char *argv[]){

	int shared_id = getSharedMemory(KEY);

	// Attach the shared memory.
	// NULL let the operating system choose where to allocate the memory
	char *shared_memory;
	if((shared_memory = shmat(shared_id, NULL, SHM_RDONLY)) == -1){
		perror("Shared memory cannot be attached");
		exit(EXIT_FAILURE);
	}

	printf("Reading shared memory:\n%s\n",shared_memory);

	// Detach
	if(shmdt(shared_memory) == -1){
		perror("Shared memory cannot be detached");
		exit(EXIT_FAILURE);
	}

	sleep(1);
	removeSharedMemory(shared_id);
	return 0;
}


int getSharedMemory(char id){
	/*
	* Generate a key
	* ftok is a hashing function that produce
	* a key by combining a file (current directory) and a byte (id)
	*/
	key_t shared_key = ftok(".", id);
	printf("Created key %X\n", shared_key);

	// Get shared memory
	int shared_id;
	if((shared_id = shmget(shared_key, SEGMENT_SIZE, IPC_CREAT | 0660)) == -1){
		perror("Cannot retrive shared memory");
		exit(EXIT_FAILURE);
	}

	return shared_id;
}

void printSharedMemoryInfo(int shared_id ){
	struct shmid_ds info;

	// IPC_STAT: get descriptor structure
	// IPC_SET: set descriptor (only permissions can be changed)
	// IPC_RMID: remove semphore set
	if(shmctl(shared_id, IPC_STAT, &info) == -1){
		perror("Cannot retrieve information about the semaphore set");
		exit(EXIT_FAILURE);
	}

	printf("------------------------------\n");
	printf("SHARED MEMORY %d\n", shared_id);
	printf("Permissions:\n\
	Owner   uid: %u\n\
	Owner   gid: %u\n\
	Creator uid: %u\n\
	Creator gid: %u\n\
	Mode       : %o\n", info.shm_perm.uid, info.shm_perm.gid, info.shm_perm.cuid, info.shm_perm.cgid, info.shm_perm.mode);
	printf("Time last attach           : %ld\n", info.shm_atime);
	printf("Time last detach           : %ld\n", info.shm_dtime);
	printf("Time last change           : %ld\n", info.shm_ctime);
	printf("Number of attached process : %lu\n", info.shm_nattch);
	printf("Segment size               : %lu\n", info.shm_segsz);
	printf("------------------------------\n");
}

void removeSharedMemory(int shared_id){
	struct shmid_ds info;

	if(shmctl(shared_id, IPC_RMID, NULL) == -1){
		perror("Cannot remove shared memory");
		exit(EXIT_FAILURE);
	}
	printf("Shared memory removed\n");
}
