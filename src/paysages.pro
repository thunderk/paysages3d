TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = \
    system \
    basics \
    definition \
    render/software \
    render/preview \
    render/opengl \
    interface/commandline \
    interface/desktop \
    interface/quick

exists( tests/googletest/sources/src/gtest-all.cc ) {
    SUBDIRS += \
        tests/googletest \
        tests
}
