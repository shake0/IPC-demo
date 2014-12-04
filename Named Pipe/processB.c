#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <string.h>

/* ----------------------------------------------------------------------------
 * Named pipes are special file that are used by processes to communicate
 *
 * The name of the file is define in PIPE_NAME and it will persist after
 * termination, it can be reused.
 *
 * Opening a named pipe is a blocking call, until both ends are connected.
 * This behavior can be override with O_NDELAY in open(). A process writting
 * a pipe with no reader will be signaled with SIGPIPE.
 *
 * File permissions in mknod are added with the current user permission.
 * Therfore using umask(0) will clean the inherited permission.
 *
 */

#define PIPE_NAME "./MyPipe"

// Process id
pid_t child_pid;

void usage();

int main(int argc, const char *argv[]){

	if(argc < 2) usage();

	// Creating the pipe
	umask(0);
	if(mknod(PIPE_NAME, S_IFIFO | 0660, 0) == -1){
		if(errno != EEXIST){
			perror("Cannot create the pipe");
			exit(EXIT_FAILURE);
		}
		else
			printf("Using existing file\n");
	}

	// Writing
	int fd = open(PIPE_NAME, O_WRONLY);
	write(fd, argv[1], strlen(argv[1]));
	close(fd);
	printf("Sent: %s\n", argv[1]);

	return 0;
}

void usage(){
	printf("./process_B <message>");
	exit(EXIT_FAILURE);
}
