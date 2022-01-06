#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "constants.h"
#define SIZE 52

void usage();


int main(int argc, char *argv[])
{

    mqd_t mqd_server, mq_client;

    struct mq_attr attr;
    char input_buff[266];
    char output_buff[266];
    struct message msg;
    struct message* res;

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = 256;
    attr.mq_curmsgs = 0;

    //open the queue for reading
    if ((mqd_server = mq_open(PROG_1_QUEUE_NAME, O_RDONLY | O_CREAT, 0660, &attr)) == -1)
    {
        perror("open queue");
        exit(EXIT_FAILURE);
    }

    printf("Executing P1\n");


    if (argc < 2)
        usage();

    //open the queue for writing
    if ((mq_client = mq_open(PROG_2_QUEUE_NAME, O_WRONLY)) == 1)
    {
        perror("open client");
    }

    //copy the content of the first argument into the message string field.
    memcpy(msg.string, argv[1], strlen(argv[1])+1);


    // send the struct
    if (mq_send(mq_client, (char *)&msg, attr.mq_msgsize, 0) == -1)
    {
        if (errno == EBADF)
        {
            ;;
        }
        else
        {
            perror("server failure");
        }
    }

    printf("Data was sent\n");


    printf("Wainting for Response from P2\n");
    //receive the data back
    if (mq_receive(mqd_server, input_buff, attr.mq_msgsize, NULL) == -1)
    {
        perror("receive");
    }
    
    int response = atoi(input_buff);

    printf("Response got\n");
    printf("Message received back: %d\n", response);
    return 0;
}

void usage(){
	printf("./a.out <message>\n");
	exit(EXIT_FAILURE);
}
