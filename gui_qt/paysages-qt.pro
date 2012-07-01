TEMPLATE = app
CONFIG += qt
QT += core gui opengl
DEPENDPATH += .
INCLUDEPATH += .
OBJECTS_DIR = ./obj/$$BUILDMODE/
MOC_DIR = ./moc/$$BUILDMODE/
DESTDIR = ../build/$$BUILDMODE/
CONFIG -= release
CONFIG += $$BUILDMODE

release:DEFINES += NDEBUG
release:QMAKE_CXXFLAGS += -Wno-unused-variable -Wno-unused-parameter -Wno-unused-but-set-variable

LIBS += -lGLU
unix:LIBS += -L$$DESTDIR -lpaysages
win32:LIBS += ../libpaysages.a -lDevIL -lILU -lILUT -lglib-2.0 -lgthread-2.0

HEADERS += $$files(*.h) $$files(../lib_paysages/*.h) $$files(../lib_paysages/shared/*.h)
SOURCES += $$files(*.cpp)
TRANSLATIONS = ../i18n/paysages_fr.ts

#system(lupdate paysages-qt.pro)
#system(lrelease $$TRANSLATIONS)
