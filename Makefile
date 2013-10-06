BUILDMODE=release
BUILDPATH=./build/${BUILDMODE}

all:build

dirs:
	mkdir -p ${BUILDPATH}

makefiles:dirs
ifeq (${BUILDMODE}, release)
	@+cd ${BUILDPATH} && qmake ../../src/paysages.pro -r -spec linux-clang
else
	@+cd ${BUILDPATH} && qmake ../../src/paysages.pro -r -spec linux-clang CONFIG+=debug CONFIG+=declarative_debug CONFIG+=qml_debug
endif

build:makefiles
	@+cd ${BUILDPATH} && $(MAKE)

clean:makefiles
	@+cd ${BUILDPATH} && $(MAKE) clean
ifeq (${BUILDMODE}, release)
	make BUILDMODE=debug clean
endif

docs:
	doxygen Doxyfile

debug:
	+make BUILDMODE=debug all

release:
	+make BUILDMODE=release all

tests:build
	LD_LIBRARY_PATH=${BUILDPATH}/rendering CK_DEFAULT_TIMEOUT=30 ${BUILDPATH}/testing/paysages-tests

run_cli:build
	LD_LIBRARY_PATH=${BUILDPATH}/rendering ${RUNNER} ${BUILDPATH}/controlling/paysages-cli

run:build
	LD_LIBRARY_PATH=${BUILDPATH}/rendering:${BUILDPATH}/exploring ${RUNNER} ${BUILDPATH}/editing/paysages-gui

profile:debug
	LD_LIBRARY_PATH=build/debug/rendering:build/debug/exploring perf record -g fp ./build/debug/paysages-gui
	perf report -g

profile_cli:debug
	LD_LIBRARY_PATH=build/debug/rendering perf record -g fp ./build/debug/paysages-cli
	perf report -g

install:release
	mkdir -p ${DESTDIR}/usr/bin
	mkdir -p ${DESTDIR}/usr/lib
	mkdir -p ${DESTDIR}/usr/share/paysages3d
	cp build/release/editing/paysages-gui ${DESTDIR}/usr/bin/paysages3d
	cp build/release/exploring/libpaysages_exploring.so ${DESTDIR}/usr/lib/
	cp build/release/rendering/libpaysages_rendering.so ${DESTDIR}/usr/lib/
	cp data/.paysages_data ${DESTDIR}/usr/share/paysages3d/
	cp -r data/i18n ${DESTDIR}/usr/share/paysages3d/
	cp -r data/images ${DESTDIR}/usr/share/paysages3d/
	cp cache/*.cache ${DESTDIR}/usr/share/paysages3d/

deb:
	apt-get install pbuilder build-essential check libdevil-dev libglib2.0-dev qt4-dev-tools
	DEBFULLNAME="Michael Lemaire" DEBEMAIL=paysages@thunderk.net dch -i -p -u low -D stable
	debuild -b -us -uc

.PHONY:all clean release build
