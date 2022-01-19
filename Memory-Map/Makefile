SRC_PROCESS_A=processA.c
EXE_PROCESS_A=a.out
SRC_PROCESS_B=processB.c
EXE_PROCESS_B=b.out
CFLAGS=


all: processA processB

processA: ${SRC_PROCESS_A}
	gcc ${CFLAGS} ${SRC_PROCESS_A} -o ${EXE_PROCESS_A}
processB: ${SRC_PROCESS_B}
	gcc ${CFLAGS} ${SRC_PROCESS_B} -o ${EXE_PROCESS_B}
clean:
	rm -f ${EXE_PROCESS_A} ${EXE_PROCESS_B} Map.txt
