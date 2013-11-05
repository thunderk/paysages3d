TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = \
    system \
    basics \
    definition \
    rendering \
    render/opengl \
    editing \
    controlling

#unix:SUBDIRS += testing tests
