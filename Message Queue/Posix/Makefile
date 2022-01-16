SRC_PROCESS_A=program_1.c
EXE_PROCESS_A=a.out
SRC_PROCESS_B=program_2.c
EXE_PROCESS_B=b.out
MQUEUE=/dev/mqueue/MQUEUE


all: processA processB

processA: ${SRC_PROCESS_A}
	gcc ${SRC_PROCESS_A} -o ${EXE_PROCESS_A} -lrt
processB: ${SRC_PROCESS_B}
	gcc ${SRC_PROCESS_B} -o ${EXE_PROCESS_B} -lrt
clean:
	rm -f ${EXE_PROCESS_A} ${EXE_PROCESS_B} ${MQUEUE}
