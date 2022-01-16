#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>
#include "constants.h"

int main(int argc, char *argv[])
{

	// mqueue object
	mqd_t mqueue;
	// mqueue attributes
	struct mq_attr attr;
	attr.mq_flags = 0;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = 256;
	attr.mq_curmsgs = 0;
	// message buffer to store incoming and outgoing messages
	char buffer[MESSAGE_BUFFER_SIZE];

	// open the queue and create it if doesn't exist
	printf("Open mqueue %s\n", MQUEUE);
	if ((mqueue = mq_open(MQUEUE, O_RDWR | O_CREAT, 0660, &attr)) == -1)
	{
		perror("open queue client");
		exit(EXIT_FAILURE);
	}

	// receive message
	printf("Waiting for message\n");
	if (mq_receive(mqueue, buffer, attr.mq_msgsize, NULL) == -1)
	{
		perror("receive");
		exit(EXIT_FAILURE);
	}
	printf("Message received: %s\n", buffer);

	// making the message all uppercase
	for(int i=0; i < strlen(buffer); ++i)
	{
		buffer[i] = toupper(buffer[i]);
	}

	// sending the acknowledge message
	printf("Sending message: %s\n", buffer);
	if (mq_send(mqueue, buffer, attr.mq_msgsize, 0) == -1)
	{
		perror("failed to send to Server");
	}

	// close mqueue descriptor
	printf("Terminating\n");
	if (mq_close(mqueue) == -1)
	{
		perror("close");
		exit(EXIT_FAILURE);
	}
	// delete mqueue
	// it may raise an error if the other process delete the queue first
	mq_unlink(MQUEUE);

	return 0;
}
