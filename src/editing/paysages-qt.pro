TEMPLATE = app
CONFIG += qt
QT += core gui opengl
DEPENDPATH += .
INCLUDEPATH += .
DESTDIR = $$PROJECT_PATH/build/$$BUILDMODE/
OBJECTS_DIR = $$DESTDIR/editing/
MOC_DIR = $$OBJECTS_DIR/moc/
CONFIG -= release
CONFIG += $$BUILDMODE
INCLUDEPATH += $$PROJECT_PATH/src/

release:DEFINES += NDEBUG
release:QMAKE_CXXFLAGS += -Wno-unused-variable -Wno-unused-parameter -Wno-unused-but-set-variable

LIBS += -lGLU
unix:LIBS += -L$$DESTDIR -lpaysages_rendering -lpaysages_exploring
win32:LIBS += ../libpaysages.a ../libpaysages_exploring.a -lDevIL -lILU -lILUT -lglib-2.0 -lgthread-2.0

HEADERS += $$files(*.h) $$files(../rendering/*.h) $$files(../rendering/shared/*.h) $$files(../exploring/*.h)
SOURCES += $$files(*.cpp)
TRANSLATIONS = $$PROJECT_PATH/data/i18n/paysages_fr.ts

#system(lupdate paysages-qt.pro)
#system(lrelease $$TRANSLATIONS)
