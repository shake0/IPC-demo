SRC_PROCESS_A=processA.c
EXE_PROCESS_A=a.out
SRC_PROCESS_B=processB.c
EXE_PROCESS_B=b.out
NAMED_PIPE=MyPipe
CFLAGS=-Wall


all: processA processB

processA: ${SRC_PROCESS_A}
	gcc ${CFLAGS} ${SRC_PROCESS_A} -o ${EXE_PROCESS_A}
processB: ${SRC_PROCESS_B}
	gcc ${CFLAGS} ${SRC_PROCESS_B} -o ${EXE_PROCESS_B}
clean:
	rm -f ${NAMED_PIPE} ${EXE_PROCESS_A} ${EXE_PROCESS_B}
