TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = \
    system \
    rendering \
    exploring \
    editing \
    controlling

unix:SUBDIRS += testing

rendering.depends = system
exploring.depends = rendering
editing.depends = exploring rendering
controlling.depends = rendering
unix:testing.depends = rendering
