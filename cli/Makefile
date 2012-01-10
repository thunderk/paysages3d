SOURCES=$(wildcard *.c)
OBJECTS=${SOURCES:.c=.o}
HEADERS=$(wildcard ../lib_paysages/shared/*.h ../lib_paysages/*.h *.h)
RESULT=paysages-cli
CC_FLAGS=-g -pg -Wall
CC_LDFLAGS=-L../lib_paysages/ -lpaysages

all:${RESULT}

clean:
	rm -f ${OBJECTS}
	rm -f ${RESULT}

%.o:%.c ${HEADERS}
	${CC} -c ${CC_FLAGS} $< -o $@

${RESULT}:${OBJECTS}
	${CC} $^ ${CC_LDFLAGS} -o $@

.PHONY:all clean

