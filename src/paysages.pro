TEMPLATE = subdirs
CONFIG += ordered

SUBDIRS = \
    system \
    rendering \
    exploring \
    editing \
    controlling \
    testing

rendering.depends = system
exploring.depends = rendering
editing.depends = exploring rendering
controlling.depends = rendering
testing.depends = rendering
