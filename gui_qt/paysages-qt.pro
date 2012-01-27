######################################################################
# Automatically generated by qmake (2.01a) dim. dc. 25 21:03:17 2011
######################################################################

TEMPLATE = app
CONFIG += qt
QT += opengl
TARGET =
DEPENDPATH += .
INCLUDEPATH += .
unix:CONFIG += debug
win32:CONFIG += release

unix:LIBS += -L../lib_paysages/ -lpaysages
win32:LIBS += ../libpaysages.a -lDevIL -lILU -lILUT -lglib-2.0 -lgthread-2.0

# Input
HEADERS += ../lib_paysages/shared/functions.h ../lib_paysages/shared/types.h \
    baseform.h \
    baseinput.h \
    dialognoise.h \
    dialogrender.h \
    dialogwanderer.h \
    formatmosphere.h \
    formclouds.h \
    formlighting.h \
    formrender.h \
    formsky.h \
    formterrain.h \
    formtextures.h \
    formwater.h \
    inputcolor.h \
    inputcolorgradation.h \
    inputdouble.h \
    inputint.h \
    inputnoise.h \
    mainwindow.h \
    preview.h \
    tools.h \
    widgetwanderer.h
FORMS +=
SOURCES += \
    baseform.cpp \
    baseinput.cpp \
    dialognoise.cpp \
    dialogrender.cpp \
    dialogwanderer.cpp \
    formatmosphere.cpp \
    formclouds.cpp \
    formlighting.cpp \
    formrender.cpp \
    formsky.cpp \
    formterrain.cpp \
    formtextures.cpp \
    formwater.cpp \
    inputcolor.cpp \
    inputcolorgradation.cpp \
    inputdouble.cpp \
    inputint.cpp \
    inputnoise.cpp \
    mainwindow.cpp \
    preview.cpp \
    widgetwanderer.cpp
