#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <string.h>

#define PENDING_QUEUE_SIZE 1 // number of process that can wait for acceptance
#define MAX_MESSAGE_SIZE 128
#define LOCAL       "./sockA"
#define DESTINATION "./sockB" 

int main(int argc, const char *argv[]){
	struct sockaddr_un address;
	int fd, connected_fd;
	char message[MAX_MESSAGE_SIZE];
	message[MAX_MESSAGE_SIZE -1] = '\0';

	// Setting my domain
	address.sun_family = AF_UNIX;
	memcpy(address.sun_path, LOCAL, strlen(LOCAL) + 1); 

	// Make a socket file
	if((fd = socket(AF_UNIX, SOCK_STREAM, 0)) == -1){
		perror("Socket cannot be instantiated");
		exit(EXIT_FAILURE);
	}

	// Bind 
	// Naming the socket so other process can address it
	unlink(LOCAL);
	if(bind(fd, (struct sockaddr*) &address, sizeof(address)) == -1){
		perror("Cannot bind socket");
		exit(EXIT_FAILURE);
	}

	// Listen
	// Set the socket as passive for accepting connection
	// On the other side connect() will succeed immidiatly if
	// the queue is not full.
	if(listen(fd, PENDING_QUEUE_SIZE) == -1){
		perror("Cannot listen");
		exit(EXIT_FAILURE);
	}

	// Accept - blocking call
	// Wait for incoming communication request
	//
	// Accepting a connection remove it from the pending requests,
	// therefore after accepting, 1 + PENDING_QUEUE_SIZE will succeed in connect()
	struct sockaddr_un agent; // The other process info
	socklen_t agent_length;
	memset(&agent, 0, sizeof(struct sockaddr_un));
	printf("Waiting for incoming connection...\n");
	if((connected_fd = accept(fd,(struct sockaddr*) &agent, &agent_length))== -1){
		perror("Cannot accept connection");
		exit(EXIT_FAILURE);
	}
	printf("Established connection with: %s\n", agent.sun_path); // it may show nothing because the socket is unbound

	// We could handle the response seperately in another thread
	// and continue accepting other requests. For purpose of clarity
	// I handle the request directly.
	char *buffer = (char*) malloc(sizeof(char)*MAX_MESSAGE_SIZE);
	size_t buffer_size = MAX_MESSAGE_SIZE;
	size_t size;
	do{
		printf("Write a message: ");
		getline(&buffer, &buffer_size, stdin);
		memset(message, 0, MAX_MESSAGE_SIZE);
		memcpy(message, buffer, strlen(buffer) -1 ); // removing \n
		// connection_fd is marked as connected 
		// and it knows where the message should be directed
		size = send(connected_fd, message, strlen(message), 0);
		printf("Sent %lu B\n", size);
	}while(strcmp(message, "quit") != 0);
	free(buffer);


	// If client is blocking in recv(): ECONNRESET
	// If client is blocking in connect(): ECONNREFUSED
	close(connected_fd);
	close(fd); 
	return 0;
}
