#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "message.h"

#define KEY 0x01

int getMessageQueue(char id);
void printMessageQueueInfo(int queue_id);
void removeEmptyQueue(int queue_id);
void usage();

int main(int argc, const char *argv[]){
	if(argc < 2) usage();

	int queue_id = getMessageQueue(KEY);

	// Print message queue parameters
	printMessageQueueInfo(queue_id);

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

void printMessageQueueInfo(int queue_id){
	struct msqid_ds info;

	// IPC_STAT: get descriptor structure
	// IPC_SET: set descriptor (only permissions can be changed)
	// IPC_RMID: remove message queue
	if(msgctl(queue_id, IPC_STAT, &info) == -1){
		perror("Cannot retrieve information about the message queue");
		exit(EXIT_FAILURE);
	}

	printf("------------------------------\n");
	printf("MESSAGE QUEUE %d\n", queue_id);
	printf("Permissions:\n\
	Owner   uid: %u\n\
	Owner   gid: %u\n\
	Creator uid: %u\n\
	Creator gid: %u\n\
	Mode       : %o\n", info.msg_perm.uid, info.msg_perm.gid, info.msg_perm.cuid, info.msg_perm.cgid, info.msg_perm.mode);
	printf("Time last message send     : %ld\n", info.msg_stime);
	printf("Time last message received : %ld\n", info.msg_rtime);
	printf("Time last change           : %ld\n", info.msg_ctime);
	printf("Number of messages         : %lu\n", info.msg_qnum);
	printf("Maximum bytes              : %lu\n", info.msg_qbytes);
	printf("PID last sender            : %d\n", info.msg_lspid);
	printf("PID last receiver          : %d\n", info.msg_lrpid);
	printf("------------------------------\n");
}

void removeEmptyQueue(int queue_id){
	struct msqid_ds info;

	if(msgctl(queue_id, IPC_STAT, &info) == -1){
		perror("Cannot retrieve information about the message queue");
		exit(EXIT_FAILURE);
	}

	if(info.msg_qnum == 0){
		if(msgctl(queue_id, IPC_RMID, NULL) == -1){
			perror("Cannot remove the message queue");
			exit(EXIT_FAILURE);
		}
	}
	printf("Message queue removed\n");
}

void usage(){
	printf("./a.out <message>\n");
	exit(EXIT_FAILURE);
}
