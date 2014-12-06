#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>

#define MAX_MESSAGE_SIZE 128
#define LOCAL       "./sockB"
#define DESTINATION "./sockA" 

int main(int argc, const char *argv[]){
	struct sockaddr_un address;
	int fd;
	char message[MAX_MESSAGE_SIZE];
	message[MAX_MESSAGE_SIZE -1] = '\0';

	// Setting my domain
	address.sun_family = AF_UNIX;
	memcpy(address.sun_path, LOCAL, strlen(LOCAL) + 1); 

	// Make a socket file
	if((fd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1){
		perror("Socket cannot be instantiated");
		exit(EXIT_FAILURE);
	}

	// Bind
	unlink(LOCAL);
	if(bind(fd, (struct sockaddr*) &address, sizeof(address)) == -1){
		perror("Cannot bind socket");
		exit(EXIT_FAILURE);
	}

	// Emitter
	struct sockaddr_un emitter;
	socklen_t len;

	size_t size;
	do{
		memset(message, 0, MAX_MESSAGE_SIZE);
		size = recvfrom(fd, message, MAX_MESSAGE_SIZE, 0, (struct sockaddr *) &emitter, &len);
		if(size == -1) { perror("Receiver"); exit(EXIT_FAILURE); }
		printf("Receive %lu B from %s\nMessage: %s\n\n", size, emitter.sun_path, message);
	}while(strcmp(message, "quit") != 0);

	return 0;
}
