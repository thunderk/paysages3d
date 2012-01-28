TEMPLATE = app
CONFIG += qt
QT += core gui opengl
DEPENDPATH += .
INCLUDEPATH += .
OBJECTS_DIR = ./obj/$$BUILDMODE/
MOC_DIR = ./moc/$$BUILDMODE/
DESTDIR = ../build/$$BUILDMODE/
CONFIG += $$BUILDMODE

unix:LIBS += -L$$DESTDIR -lpaysages
win32:LIBS += $$DESTDIR/libpaysages.a -lDevIL -lILU -lILUT -lglib-2.0 -lgthread-2.0

HEADERS += $$files(*.h) $$files(../lib_paysages/*.h) $$files(../lib_paysages/shared/*.h)
SOURCES += $$files(*.cpp)
