TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = \
    system \
    rendering \
    exploring \
    editing \
    controlling \
    definition \
    basics

unix:SUBDIRS += testing

rendering.depends = system
exploring.depends = rendering
definition.depends = rendering system
editing.depends = exploring rendering
controlling.depends = rendering
unix:testing.depends = rendering
