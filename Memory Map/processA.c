#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#define FILE_TO_MAP "./Map.txt"
#define MAPPING_SIZE 518

int main(int argc, const char *argv[]){
	int fd, i;
	char *mapped;
	struct flock lock;

	// SEEK_SET: beginning of the file
	// SEEK_CUR: current position
	// SEEK_END: end of the file
	lock.l_whence = SEEK_SET;
	// starting offset;
	lock.l_start = 0;
	// locking the whole file
	lock.l_len = 0;
	// type
	lock.l_type = F_WRLCK;

	fd = open(FILE_TO_MAP, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
	if(fd == -1){
		perror("File cannot be open");
		exit(EXIT_FAILURE);
	}
	struct stat fd_info;
	if(fstat(fd, &fd_info) == -1){
		perror("File description cannot be retrieved");
		exit(EXIT_FAILURE);
	}
	if(fd_info.st_size == 0){
		fprintf(stderr, "ERROR: Mapping an empty file\n"); // Leads to ERROR BUS
		for(i = 0; i < MAPPING_SIZE; i++){
			write(fd, "-", 1);
		}
	}
	else if(fd_info.st_size < MAPPING_SIZE){
		fprintf(stderr, "WARNING: Mapping a segment of %dB but the file is only %dB.\nThe extra data will be discarded when unmapped.\n",
				MAPPING_SIZE, fd_info.st_size);
	}


	mapped = mmap( NULL, MAPPING_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	if(mapped == (char *)-1){
		perror("File cannot be mapped");
		exit(EXIT_FAILURE);
	}

	// This are advisory lock, nothing stops you
	// from messing around by writing the file without
	// requesting a lock
	//
	// F_SETLKW: Set lock blocking
	// F_SETLK: Set lock non-blocking
	// F_GETLK: Test lock
	fcntl(fd, F_SETLKW, &lock);
	printf("Writing to the file...\n");

	for(i = 0; i < MAPPING_SIZE; i++){
		mapped[i] = '*';
	}

	printf("Press ENTER to release the lock");
	getc(stdin);
	// release
	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLKW, &lock);

	lock.l_type = F_RDLCK;
	fcntl(fd, F_SETLKW, &lock);

	sleep(1);

	lock.l_type = F_UNLCK;
	fcntl(fd, F_SETLKW, &lock);

	munmap(mapped, MAPPING_SIZE);
	close(fd);

	return 0;
}
