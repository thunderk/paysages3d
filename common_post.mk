OBJECTS = ${SOURCES:%.c=${OBJPATH}/%.o}

ifeq (${BUILDMODE},debug)
	CC_FLAGS += -g -pg
	CC_LDFLAGS += -g -pg
endif
ifeq (${BUILDMODE},release)
	CC_FLAGS += -O3 -DNDEBUG -Wno-unused-variable -Wno-unused-but-set-variable
endif

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
