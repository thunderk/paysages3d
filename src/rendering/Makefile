include ../common_pre.mk

OBJPATH = ${BUILDPATH}/rendering
RESULT = ${BUILDPATH}/libpaysages_rendering.so
SOURCES += $(wildcard *.c atmosphere/*.c clouds/*.c terrain/*.c textures/*.c water/*.c tools/*.c shared/*.c)

LIBS += glib-2.0 gthread-2.0 IL ILU
CC_FLAGS += -DHAVE_GLIB=1
CC_LDFLAGS += -shared

CHECK_OPENCL = $(shell pkg-config --modversion --silence-errors OpenCL)
ifneq (,${CHECK_OPENCL})
	LIBS += OpenCL
	CC_FLAGS += -DHAVE_OPENCL=1
endif

include ../common_post.mk
