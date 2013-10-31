TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = \
    system \
    basics \
    definition \
    rendering \
    exploring \
    editing \
    controlling

unix:SUBDIRS += testing tests

basics.depends = system
definition.depends = basics
rendering.depends = definition
exploring.depends = rendering
editing.depends = exploring rendering
controlling.depends = rendering
unix:testing.depends = rendering
unix:tests.depends = rendering
