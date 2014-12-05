#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <linux/ipc.h>
#include <linux/msg.h>
#include "message.h"

#define KEY 0x01

int getMessageQueue(char id);
void usage();

int main(int argc, const char *argv[]){
	if(argc < 2)
		usage();

	printf("The maximum message size is %d bytes\n", MSGMAX);

	int queue_id = getMessageQueue(KEY);

	// Preparing the message
	struct message m1;
	m1.type = 1; // Can be use as a label for multiplexing
	m1.sender = getpid();
	memcpy(m1.message, argv[1], strlen(argv[1]) +1);

	// Enqueue the message
	if(msgsnd(queue_id, &m1, sizeof(struct message) - sizeof(long), 0) == -1){
		perror("Cannot send message");
		exit(EXIT_FAILURE);
	}
	printf("Message sent: %s\n", argv[1]);

	return 0;
}

int getMessageQueue(char id){
	/*
	* Generate a key
	* ftok is a hashing function that produce
	* a key by combining a file (current directory) and a byte (id)
	*/
	key_t queue_key = ftok(".", id);
	printf("Created key %X\n", queue_key);

	// Get message queue
	int queue_id;
	if((queue_id = msgget(queue_key, IPC_CREAT | 0660)) == -1){
		perror("Cannot retrive queue");
		exit(EXIT_FAILURE);
	}
	return queue_id;
}

void usage(){
	printf("./process_B <message>");
	exit(EXIT_FAILURE);
}
