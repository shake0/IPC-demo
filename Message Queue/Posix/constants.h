#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <mqueue.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define PROG_1_QUEUE_NAME "/QUEUE_PROG_1"
#define PROG_2_QUEUE_NAME "/QUEUE_PROG_2"
#define STRING_SIZE 5
#define MAX_MSG_SIZE  256
#define MAX_MSG 50
#define BUFF_SIZE MAX_MSG_SIZE + 50

int flag = 0;
int firstFivesSent = 0;

struct message {
	char string[5];
};
