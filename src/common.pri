INCLUDEPATH += ..

CONFIG(debug) {
    DESTDIR = ../../build/debug/
    MAKEFILE = Makefile.debug
} else {
    DESTDIR = ../../build/release/
    MAKEFILE = Makefile.release
}

release:DEFINES += NDEBUG

