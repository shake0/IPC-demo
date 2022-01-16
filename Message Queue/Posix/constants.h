#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <mqueue.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#define MESSAGE_BUFFER_SIZE 256
#define MQUEUE "/MQUEUE"
