BUILDMODE=debug
BUILDPATH=../build/${BUILDMODE}
SOURCES=$(wildcard *.c)
OBJECTS=${SOURCES:%.c=obj/%.o}
HEADERS=$(wildcard ../lib_paysages/shared/*.h ../lib_paysages/*.h *.h)
RESULT=${BUILDPATH}/paysages-cli
CC_FLAGS=-g -pg -Wall
CC_LDFLAGS=-L${BUILDPATH} -lpaysages

all:prepare ${RESULT}

prepare:
	mkdir -p ./obj
	mkdir -p ${BUILDPATH}

clean:
	rm -f ${OBJECTS}
	rm -f ${RESULT}

obj/%.o:%.c ${HEADERS}
	${CC} -c ${CC_FLAGS} $< -o $@

${RESULT}:${OBJECTS}
	${CC} $^ ${CC_LDFLAGS} -o $@

.PHONY:all clean prepare

