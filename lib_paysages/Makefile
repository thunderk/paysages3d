BUILDMODE = debug
BUILDPATH = ../build/${BUILDMODE}
OBJPATH = ./obj/${BUILDMODE}
SOURCES = $(wildcard *.c atmosphere/*.c)
OBJECTS = ${SOURCES:%.c=${OBJPATH}/%.o}
HEADERS = $(wildcard shared/*.h atmosphere/*.h *.h)
RESULT = ${BUILDPATH}/libpaysages.so
CC_FLAGS = -Wall -fPIC $(shell pkg-config --cflags glib-2.0 gthread-2.0) -DHAVE_GLIB=1
CC_LDFLAGS = $(shell pkg-config --libs glib-2.0 gthread-2.0) -lIL -lILU

ifeq ($(BUILDMODE),debug)
	CC_FLAGS += -g -pg
	CC_LDFLAGS += -g -pg
endif
ifeq ($(BUILDMODE),release)
	CC_FLAGS += -O3 -DNDEBUG -Wno-unused-variable -Wno-unused-but-set-variable
endif

all:prepare ${RESULT}
	
prepare:
	mkdir -p ${OBJPATH}
	mkdir -p ${BUILDPATH}

clean:
	rm -f ${OBJECTS}
	rm -f ${RESULT}

${OBJPATH}/%.o:%.c ${HEADERS}
	mkdir -p `dirname $@`
	${CC} -c ${CC_FLAGS} $< -o $@

${RESULT}:${OBJECTS}
	${CC} $^ -shared ${CC_LDFLAGS} -o $@

.PHONY:all clean prepare

