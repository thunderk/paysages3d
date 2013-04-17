include ../common_pre.mk

OBJPATH = ${BUILDPATH}/controlling
RESULT = ${BUILDPATH}/paysages-cli
SOURCES += $(wildcard *.c)
HEADERS += $(wildcard *.h)

CC_LDFLAGS += -L${BUILDPATH} -lpaysages_rendering

include ../common_post.mk
