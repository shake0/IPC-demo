#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "constants.h"

int main(int argc, char *argv[])
{

	mqd_t mqd_server, mq_client;

	struct mq_attr attr;
	char input_buff[266];
	char output_buff[266];

	attr.mq_flags = 0;
	attr.mq_maxmsg = 10;
	attr.mq_msgsize = 256;
	attr.mq_curmsgs = 0;

	struct message *msg;
	struct message res;

	char buff[266];

	if ((mq_client = mq_open(PROG_2_QUEUE_NAME, O_RDONLY | O_CREAT, 0660, &attr)) == -1)
	{
		perror("open queue client");
		exit(EXIT_FAILURE);
	}

	if ((mqd_server = mq_open(PROG_1_QUEUE_NAME, O_WRONLY)) == -1)
	{
		perror("open queue server");
		exit(EXIT_FAILURE);
	}

	printf("Executing P2\n");

	if (mq_receive(mq_client, buff, attr.mq_msgsize, NULL) == -1)
	{
		perror("receive");
		exit(EXIT_FAILURE);
	}
	msg = (struct message *)buff;
   

	printf("Message Received: %s\n", msg->string);

		
	printf("Trying to send aknowledge...\n");

	snprintf(output_buff, strlen(output_buff), "%d", getpid());

	if (mq_send(mqd_server, (char *)&res, attr.mq_msgsize, 0) == -1)
	{
		perror("failed to send to Server");
	}

	printf("Response sent back\n");

	if (mq_close(mq_client) == -1)
	{
		perror("close");
		exit(EXIT_FAILURE);
	}
	if (mq_unlink(PROG_2_QUEUE_NAME) == -1)
	{
		perror("unlike");
		exit(EXIT_FAILURE);
	}

	return 0;
}
