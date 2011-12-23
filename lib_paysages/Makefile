SOURCES=$(wildcard *.c)
OBJECTS=${SOURCES:.c=.o}
HEADERS=$(wildcard shared/*.h *.h)
RESULT=libpaysages.so
CC_FLAGS=-g -pg -Wall $(shell pkg-config --cflags glib-2.0)
CC_LDFLAGS=$(shell pkg-config --libs glib-2.0) -lIL -lILU

all:${RESULT}

clean:
	rm -f ${OBJECTS}
	rm -f ${RESULT}

%.o:%.c ${HEADERS}
	${CC} -c ${CC_FLAGS} $< -o $@

${RESULT}:${OBJECTS}
	${CC} $^ -shared ${CC_LDFLAGS} -o $@

.PHONY:all clean

