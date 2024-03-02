TARGET_1 = main
SRCS_1 = list.h list.c main.c
TARGET_2 = main_spin
SRCS_2 = list_spin.h list_spin.c main_spin.c
TARGET_2 = main_rwlock
SRCS_2 = list_rwlock.h list_rwlock.c main_rwlock.c

CC=gcc
RM=rm
CFLAGS= -g -Wall
LIBS=-lpthread
INCLUDE_DIR="."

all: ${TARGET_1} ${TARGET_2}

${TARGET_1}: ${SRCS_1}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS_1} ${LIBS} -o ${TARGET_1}
${TARGET_2}: ${SRCS_2}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS_2} ${LIBS} -o ${TARGET_2}
${TARGET_3}: ${SRCS_3}
	${CC} ${CFLAGS} -I${INCLUDE_DIR} ${SRCS_3} ${LIBS} -o ${TARGET_3}

clean:
	${RM} -f *.o ${TARGET_1}
