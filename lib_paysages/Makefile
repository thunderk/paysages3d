BUILDMODE = debug
BUILDPATH = ../build/${BUILDMODE}
OBJPATH = ./obj/${BUILDMODE}
SOURCES = $(wildcard *.c atmosphere/*.c clouds/*.c terrain/*.c water/*.c tools/*.c)
OBJECTS = ${SOURCES:%.c=${OBJPATH}/%.o}
HEADERS = $(wildcard *.h atmosphere/*.h clouds/*.h terrain/*.h water/*.h tools/*.h shared/*.h)
RESULT = ${BUILDPATH}/libpaysages.so
LIBS = glib-2.0 gthread-2.0 IL ILU
CC_FLAGS = -Wall -fPIC -DHAVE_GLIB=1

CHECK_OPENCL = $(shell pkg-config --modversion --silence-errors OpenCL)
ifneq (,${CHECK_OPENCL})
	LIBS += OpenCL
	CC_FLAGS += -DHAVE_OPENCL=1
endif

CC_FLAGS += $(shell pkg-config --cflags ${LIBS})
CC_LDFLAGS = $(shell pkg-config --libs ${LIBS})

ifeq (${BUILDMODE},debug)
	CC_FLAGS += -g -pg
	CC_LDFLAGS += -g -pg
endif
ifeq (${BUILDMODE},release)
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

