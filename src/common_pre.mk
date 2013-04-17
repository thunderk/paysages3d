ifeq ($(strip $(BUILDMODE)),)
    exit 1
endif
ifeq ($(strip $(PROJECT_PATH)),)
    exit 1
endif

PATH_LIB_PAYSAGES = ${PROJECT_PATH}/rendering
BUILDMODE = debug
BUILDPATH = ${PROJECT_PATH}/build/${BUILDMODE}
HEADERS = $(${PATH_LIB_PAYSAGES}/atmosphere/*.h ${PATH_LIB_PAYSAGES}/clouds/*.h ${PATH_LIB_PAYSAGES}/terrain/*.h ${PATH_LIB_PAYSAGES}/textures/*.h ${PATH_LIB_PAYSAGES}/water/*.h ${PATH_LIB_PAYSAGES}/tools/*.h ${PATH_LIB_PAYSAGES}/shared/*.h)
