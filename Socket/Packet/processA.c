#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>

#define MAX_MESSAGE_SIZE 128
#define LOCAL       "./sockA"
#define DESTINATION "./sockB" 

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

	// Destination
	struct sockaddr_un destination;
	destination.sun_family = AF_UNIX;
	memcpy(destination.sun_path, DESTINATION, strlen(DESTINATION) + 1); 

	char *buffer = (char*) malloc(sizeof(char)*MAX_MESSAGE_SIZE);
	size_t buffer_size = MAX_MESSAGE_SIZE;
	size_t size;
	do{
		printf("Write a message: ");
		getline(&buffer, &buffer_size, stdin);
		memset(message, 0, MAX_MESSAGE_SIZE);
		memcpy(message, buffer, strlen(buffer) -1 ); // removing \n
		size = sendto(fd, message, strlen(message), 0, (struct sockaddr*) &destination, sizeof(destination));
		printf("Sent %lu B\n", size);
	}while(strcmp(message, "quit") != 0);
	free(buffer);

	return 0;
}
