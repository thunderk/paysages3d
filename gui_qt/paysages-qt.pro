######################################################################
# Automatically generated by qmake (2.01a) dim. dc. 25 21:03:17 2011
######################################################################

TEMPLATE = app
TARGET =
DEPENDPATH += .
INCLUDEPATH += .
unix:CONFIG += debug
win32:CONFIG += release

unix:LIBS += -L../lib_paysages/ -lpaysages
win32:LIBS += ../libpaysages.a -lDevIL -lILU -lILUT -lglib-2.0 -lgthread-2.0

# Input
HEADERS += ../lib_paysages/shared/functions.h ../lib_paysages/shared/types.h \
    mainwindow.h \
    formwater.h \
    preview.h \
    baseform.h \
    inputdouble.h \
    baseinput.h \
    inputcolor.h \
    formrender.h \
    inputint.h \
    dialogrender.h \
    dialognoise.h \
    inputcolorgradation.h \
    formsky.h \
    formnoise.h \
    inputnoise.h \
    tools.h
FORMS +=
SOURCES += \
    mainwindow.cpp \
    formwater.cpp \
    preview.cpp \
    baseform.cpp \
    inputdouble.cpp \
    baseinput.cpp \
    inputcolor.cpp \
    formrender.cpp \
    inputint.cpp \
    dialogrender.cpp \
    dialognoise.cpp \
    inputcolorgradation.cpp \
    formsky.cpp \
    formnoise.cpp \
    inputnoise.cpp
