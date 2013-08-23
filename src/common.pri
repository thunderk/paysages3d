INCLUDEPATH += ..

CONFIG(declarative_debug) {
    DESTDIR = ../../build/debug/
    MAKEFILE = Makefile.debug
} else {
    DESTDIR = ../../build/release/
    MAKEFILE = Makefile.release

    DEFINES += NDEBUG
}
