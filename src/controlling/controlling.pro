TEMPLATE = app
CONFIG += console
CONFIG -= app_bundle
CONFIG -= qt

TARGET = paysages-cli

INCLUDEPATH += $$PWD/..

CONFIG(release, debug|release): DEFINES += NDEBUG

SOURCES += main.c

win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../rendering/release/ -lpaysages_rendering
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../rendering/debug/ -lpaysages_rendering
else:unix: LIBS += -L$$OUT_PWD/../rendering/ -lpaysages_rendering
DEPENDPATH += $$PWD/../rendering
