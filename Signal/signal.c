#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/signal.h>

void receiveSignal();
void sendSignal();
void printSignals();

// Signal callback
// This function can be call at anytime during execution
// then it is suggested that it use async safe calls
// as described in the `man signal` page.
void handler(int interrupt_number);

sig_atomic_t stop  = 0;
pid_t        child = 0;

int main(int argc, const char *argv[]){
	// Fork
	child = fork();
	if(child == 0)
		receiveSignal();
	else
		sendSignal();
	
	return 0;
}

void handler(int interrupt_number){
	write(fileno(stderr), "CHILD Signal received: ", strlen("CHILD Signal received: "));
	switch(interrupt_number){
		case SIGABRT:
			write(fileno(stderr), "SIGABRT", strlen("SIGABRT"));
			break;
		case SIGCONT:
			write(fileno(stderr), "SIGCONT", strlen("SIGCONT"));
			break;
		case SIGINT:
			write(fileno(stderr), "SIGINT",  strlen("SIGINT") );
			stop = 1;
			break;
		case SIGQUIT:
			write(fileno(stderr), "SIGQUIT", strlen("SIGQUIT"));
			stop = 1;
			break;
		case SIGTSTP:
			write(fileno(stderr), "SIGTSTP", strlen("SIGTSTP"));
			stop = 1;
			break;
		case SIGSTOP:
			write(fileno(stderr), "SIGSTOP", strlen("SIGSTOP"));
			stop = 1;
			break;
		case SIGTERM:
			write(fileno(stderr), "SIGTERM", strlen("SIGTERM"));
			stop = 1;
			break;
		case SIGUSR1:
			write(fileno(stderr), "SIGUSR1", strlen("SIGUSR1"));
			break;
		case SIGUSR2:
			write(fileno(stderr), "SIGUSR2", strlen("SIGUSR2"));
			break;
	}
	write(fileno(stderr), "\n", 1);
}

// Process B redefine the handle for various signals
void receiveSignal(){
	// Wrap handler function in sigaction
	struct sigaction my_action;
	my_action.sa_handler = &handler;
	// Interupt can happen during other calls (open, read, ...)
	// with this flag the operation is restarted, otherwise it will return EINTR
	my_action.sa_flags = SA_RESTART; 

	// Attach signal to handler
	sigaction(SIGABRT, &my_action, NULL);
	sigaction(SIGCONT, &my_action, NULL); // resume (eg. fg)
	sigaction(SIGINT,  &my_action, NULL); // '^C'
	sigaction(SIGQUIT, &my_action, NULL); // '^\'
	sigaction(SIGSTOP, &my_action, NULL);
	sigaction(SIGTSTP, &my_action, NULL); // '^Z'
	sigaction(SIGTERM, &my_action, NULL);
	sigaction(SIGUSR1, &my_action, NULL);
	sigaction(SIGUSR2, &my_action, NULL);

	// Pretend to work
	while(!stop) sleep(10);
}

// Process A send signal to B
// In order to do that, A need to know the PID of B
void sendSignal(){
	printSignals();
	while(!stop){
		printf("Send signal: ");
		char c;
		while((c = getc(stdin)) == '\n');
		switch(c){
			case '1':
				kill(child, SIGABRT);
				break;
			case '2':
				kill(child, SIGCONT);
				break;
			case '3':
				kill(child, SIGINT);
				stop = 1;
				break;
			case '4':
				kill(child, SIGQUIT);
				break;
			case '5':
				kill(child, SIGSTOP);
				break;
			case '6':
				kill(child, SIGTERM);
				break;
			case '7':
				kill(child, SIGUSR1);
				break;
			case '8':
				kill(child, SIGUSR2);
				break;
			default:
				printSignals();
				printf("\n\n");
		}
		usleep(10000); // Give process B the time to print since we are sharing the same tty
	}
}

void printSignals(){
	printf("1: SIGABRT\n2: SIGCONT\n3: SIGINT\n4: SIGQUIT\n5: SIGSTOP\n6: SIGTSTP\n7: SIGTERM\n8: SIGUSR1\n9: SIGUSR2\n");
}
