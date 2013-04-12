OBJECTS = ${SOURCES:%.c=${OBJPATH}/%.o}

ifeq (${BUILDMODE},debug)
	CC_FLAGS += -g -pg
	CC_LDFLAGS += -g -pg
endif
ifeq (${BUILDMODE},release)
	CC_FLAGS += -O3 -DNDEBUG -Wno-unused-variable -Wno-unused-but-set-variable
endif

ifneq (,${LIBS})
	CC_FLAGS += $(shell pkg-config --cflags ${LIBS})
	CC_LDFLAGS += $(shell pkg-config --libs ${LIBS})
endif

CC_FLAGS += -fPIC -Wall
CC_LDFLAGS += -fPIC

all:prepare ${RESULT}

prepare:
	mkdir -p ${BUILDPATH}

clean:
	rm -f ${OBJECTS}
	rm -f ${RESULT}

${OBJPATH}/%.o:%.c ${HEADERS}
	mkdir -p `dirname $@`
	${CC} -c ${CC_FLAGS} $< -o $@

${RESULT}:${OBJECTS}
	${CC} $^ ${CC_LDFLAGS} -o $@

.PHONY:all clean prepare
