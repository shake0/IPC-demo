#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "constants.h"
#define SIZE 52

void usage(){
	printf("./a.out <message>\n");
	exit(EXIT_FAILURE);
}

void noop(int interrupt) {} // noop interrupt handler

int main(int argc, char *argv[])
{
	if (argc < 2) usage();

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
	// signal interrupt
	int sig;
	sigset_t signalset;
	struct sigevent event;

	// open the queue and create it if doesn't exist
	printf("Open mqueue %s\n", MQUEUE);
	if ((mqueue = mq_open(MQUEUE, O_RDWR | O_CREAT, 0660, &attr)) == -1)
	{
		perror("open queue");
		exit(EXIT_FAILURE);
	}

	// copy the content of the first argument into the message string field.
	strcpy(buffer, argv[1]);

	// send the message
	printf("Sending message: %s\n", buffer);
	if (mq_send(mqueue, buffer, attr.mq_msgsize, 0) == -1)
	{
		perror("sending message");
		exit(EXIT_FAILURE);
	}

	// wait for the other process to receive our message.
	// this is to avoid to read our own message.
	// Please note mq_notify only notifies you when there is a new message
	// and the queue was empty.
	// 
	// register for mqueue notification requesting to receive SIGUSR1
	event.sigev_notify = SIGEV_SIGNAL;
	event.sigev_signo = SIGUSR1;
	mq_notify(mqueue, &event);

	// set the interrupt handler to catch SIGUSR1, otherwise 
	// it will trigger the default behaviour (terminate)
	struct sigaction signal_action;
	signal_action.sa_handler = &noop;
	signal_action.sa_flags = SA_RESTART;
	sigaction(SIGUSR1, &signal_action, NULL);

	// wait for SIGUSR1 signal
	sigemptyset(&signalset);
	sigaddset(&signalset, SIGUSR1);
	printf("Waiting for signal\n");
	if(sigwait(&signalset, &sig) != 0)
	{
		perror("sigwait");
	}
	
	// receive ack message
	printf("Waiting for message\n");
	if (mq_receive(mqueue, buffer, attr.mq_msgsize, NULL) == -1)
	{
		perror("receiving message");
		exit(EXIT_FAILURE);
	}
	printf("Message received: %s\n", buffer);

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
