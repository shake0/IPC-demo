#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>
#include <errno.h>

#define MAX_MESSAGE_SIZE 128
#define LOCAL       "./sockB"
#define DESTINATION "./sockA" 

int main(int argc, const char *argv[]){
	struct sockaddr_un address;
	int fd;
	char message[MAX_MESSAGE_SIZE];
	message[MAX_MESSAGE_SIZE -1] = '\0';

	// Make a socket file
	if((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
		perror("Socket cannot be instantiated");
		exit(EXIT_FAILURE);
	}

	// Destination
	address.sun_family = AF_UNIX;
	memcpy(address.sun_path, DESTINATION, strlen(DESTINATION) + 1);

	// Connect - blocking call
	// STREAM are need to connect to the other hand before
	// starting a transfer.
	//
	// The file descriptor is marked as connected
	printf("Connecting...\n");
	if(connect(fd, (struct sockaddr*) &address, sizeof(address)) == -1){
		if(errno == ECONNREFUSED) printf("ECONNREFUSED\n");
		close(fd);
		perror("Cannot connect");
		exit(EXIT_FAILURE);
	}
	printf("Established connection with: %s\n", address.sun_path);

	// Receiving
	size_t size;
	do{
		memset(message, 0, MAX_MESSAGE_SIZE);
		size = recv(fd, message, MAX_MESSAGE_SIZE, 0);
		if(size == -1) { 
			if(errno == ECONNRESET) printf("ECONNRESET\n");
			close(fd);
			perror("Receiver"); exit(EXIT_FAILURE); 
		}
		printf("Receive %lu B\nMessage: %s\n\n", size, message);
	}while(strcmp(message, "quit") != 0);

	close(fd);

	return 0;
}
