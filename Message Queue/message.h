#include <sys/types.h>

// Shared message structure
// extends the basic struct msgbuf
// NOTE: pointer should not be passed. (they work only on sender's address space)

struct message {
	long type; // mandatory holder for the original mtype
	pid_t sender;
	char message[128];
};
