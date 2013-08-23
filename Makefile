BUILDMODE=release
BUILDPATH=./build/${BUILDMODE}
QBUILDMODE=declarative_${BUILDMODE}
CC=gcc
MAKE=make -f Makefile.${BUILDMODE}

ifneq (,$(COMPILER))
	CC=$(COMPILER)
endif

all:dirs makefiles
	@+cd src/rendering && $(MAKE)
	@+cd src/exploring && $(MAKE)
	@+cd src/controlling && $(MAKE)
	@+cd src/editing && $(MAKE)
	@+cd src/testing && $(MAKE)

dirs:
	mkdir -p ${BUILDPATH}/rendering
	mkdir -p ${BUILDPATH}/exploring
	mkdir -p ${BUILDPATH}/controlling
	mkdir -p ${BUILDPATH}/editing
	mkdir -p ${BUILDPATH}/testing

makefiles:
	@+cd src/rendering && qmake rendering.pro "CONFIG=$(QBUILDMODE)" -r -spec linux-clang
	@+cd src/exploring && qmake exploring.pro "CONFIG=$(QBUILDMODE)" -r -spec linux-clang
	@+cd src/controlling && qmake controlling.pro "CONFIG=$(QBUILDMODE)" -r -spec linux-clang
	@+cd src/editing && qmake editing.pro "CONFIG=${QBUILDMODE}" -r -spec linux-clang
	@+cd src/testing && qmake testing.pro "CONFIG=$(QBUILDMODE)" -r -spec linux-clang

clean:
	@+cd src/rendering && qmake "CONFIG=$(QBUILDMODE)" && $(MAKE) clean
	@+cd src/exploring && qmake "CONFIG=$(QBUILDMODE)" && $(MAKE) clean
	@+cd src/controlling && qmake "CONFIG=$(QBUILDMODE)" && $(MAKE) clean
	@+cd src/editing && qmake "CONFIG=${QBUILDMODE}" && $(MAKE) clean
	@+cd src/testing && qmake "CONFIG=$(QBUILDMODE)" && $(MAKE) clean
	rm -f ${BUILDPATH}/paysages-cli
	rm -f ${BUILDPATH}/paysages-gui
	rm -f ${BUILDPATH}/paysages-tests
	rm -f ${BUILDPATH}/libpaysages_exploring.so*
	rm -f ${BUILDPATH}/libpaysages_rendering.so*
ifeq (${BUILDMODE}, release)
	+make BUILDMODE=debug clean
	rm -f src/rendering/Makefile*
	rm -f src/exploring/Makefile*
	rm -f src/controlling/Makefile*
	rm -f src/editing/Makefile*
	rm -f src/testing/Makefile*
endif

docs:
	doxygen Doxyfile

debug:
	+make BUILDMODE=debug all

release:
	+make BUILDMODE=release all

tests: all
	LD_LIBRARY_PATH=${BUILDPATH} CK_DEFAULT_TIMEOUT=30 ${BUILDPATH}/paysages-tests

run_cli: all
	LD_LIBRARY_PATH=${BUILDPATH} ${RUNNER} ${BUILDPATH}/paysages-cli

run: all
	LD_LIBRARY_PATH=${BUILDPATH} ${RUNNER} ${BUILDPATH}/paysages-gui

profile: debug
	LD_LIBRARY_PATH=build/debug perf record -g fp ./build/debug/paysages-gui
	perf report -g

profile_cli: debug
	LD_LIBRARY_PATH=build/debug perf record -g fp ./build/debug/paysages-cli
	perf report -g

install:release
	mkdir -p ${DESTDIR}/usr/bin
	mkdir -p ${DESTDIR}/usr/lib
	mkdir -p ${DESTDIR}/usr/share/paysages3d
	cp build/release/paysages-gui ${DESTDIR}/usr/bin/paysages3d
	cp build/release/libpaysages_exploring.so ${DESTDIR}/usr/lib/
	cp build/release/libpaysages_rendering.so ${DESTDIR}/usr/lib/
	cp data/.paysages_data ${DESTDIR}/usr/share/paysages3d/
	cp -r data/i18n ${DESTDIR}/usr/share/paysages3d/
	cp -r data/images ${DESTDIR}/usr/share/paysages3d/
	cp cache/*.cache ${DESTDIR}/usr/share/paysages3d/

deb:
	apt-get install pbuilder build-essential check libdevil-dev libglib2.0-dev qt4-dev-tools
	DEBFULLNAME="Michael Lemaire" DEBEMAIL=paysages@thunderk.net dch -i -p -u low -D stable
	debuild -b -us -uc

.PHONY:all clean release
