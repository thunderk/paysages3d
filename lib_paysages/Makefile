include ../common_pre.mk

OBJPATH = ${BUILDPATH}/lib_paysages
RESULT = ${BUILDPATH}/libpaysages.so
SOURCES += $(wildcard *.c atmosphere/*.c clouds/*.c terrain/*.c textures/*.c water/*.c tools/*.c shared/*.c)

LIBS += glib-2.0 gthread-2.0 IL ILU
CC_FLAGS += -DHAVE_GLIB=1
CC_LDFLAGS += -shared

CHECK_OPENCL = $(shell pkg-config --modversion --silence-errors OpenCL)
ifneq (,${CHECK_OPENCL})
	LIBS += OpenCL
	CC_FLAGS += -DHAVE_OPENCL=1
endif

CC_FLAGS += $(shell pkg-config --cflags ${LIBS})
CC_LDFLAGS += $(shell pkg-config --libs ${LIBS})

include ../common_post.mk
