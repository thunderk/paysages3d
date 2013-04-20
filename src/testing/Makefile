include ../common_pre.mk

OBJPATH = ${BUILDPATH}/testing
RESULT = ${BUILDPATH}/paysages-tests
SOURCES += $(wildcard *.c)
HEADERS += $(wildcard *.h)

CC_LDFLAGS += -L${BUILDPATH} -lpaysages_rendering
LIBS += check

include ../common_post.mk
