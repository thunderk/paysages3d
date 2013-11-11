#-------------------------------------------------
#
# Project created by QtCreator 2013-11-10T17:45:22
#
#-------------------------------------------------

QT       -= core gui

TARGET = googletest
TEMPLATE = lib

DEFINES += GOOGLETEST_LIBRARY

QMAKE_CXXFLAGS += -pthread -Wno-missing-field-initializers

INCLUDEPATH += . ./include

SOURCES += \
    src/gtest-all.cc

HEADERS +=

unix:!symbian {
    maemo5 {
        target.path = /opt/usr/lib
    } else {
        target.path = /usr/lib
    }
    INSTALLS += target
}
