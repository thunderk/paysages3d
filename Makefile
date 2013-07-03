BUILDMODE=release
BUILDPATH=./build/${BUILDMODE}
CC=gcc
MAKE=make

ifneq (,$(COMPILER))
	CC=$(COMPILER)
endif

all:
	@+cd src/rendering && $(MAKE) CC=${CC} BUILDMODE=${BUILDMODE} PROJECT_PATH=${CURDIR}
	@+cd src/exploring && $(MAKE) CC=${CC} BUILDMODE=${BUILDMODE} PROJECT_PATH=${CURDIR}
	@+cd src/controlling && $(MAKE) CC=${CC} BUILDMODE=${BUILDMODE} PROJECT_PATH=${CURDIR}
	@+cd src/editing && qmake "BUILDMODE=${BUILDMODE}" "PROJECT_PATH=${CURDIR}" && $(MAKE)
	@+cd src/testing && $(MAKE) CC=${CC} BUILDMODE=${BUILDMODE} PROJECT_PATH=${CURDIR}

clean:
	cd src/rendering && $(MAKE) clean BUILDMODE=${BUILDMODE} PROJECT_PATH=${CURDIR}
	cd src/exploring && $(MAKE) clean BUILDMODE=${BUILDMODE} PROJECT_PATH=${CURDIR}
	cd src/controlling && $(MAKE) clean BUILDMODE=${BUILDMODE} PROJECT_PATH=${CURDIR}
	cd src/editing && qmake "BUILDMODE=${BUILDMODE}" "PROJECT_PATH=${CURDIR}" && $(MAKE) clean
	cd src/testing && $(MAKE) clean BUILDMODE=${BUILDMODE} PROJECT_PATH=${CURDIR}
	rm -f ${BUILDPATH}/paysages-cli
	rm -f ${BUILDPATH}/paysages-qt
	rm -f ${BUILDPATH}/paysages-tests
	rm -f ${BUILDPATH}/libpaysages_exploring.so
	rm -f ${BUILDPATH}/libpaysages_rendering.so

docs:
	doxygen Doxyfile

debug:
	make BUILDMODE=debug all

release:
	make BUILDMODE=release all

tests: all
	LD_LIBRARY_PATH=${BUILDPATH} CK_DEFAULT_TIMEOUT=30 ${BUILDPATH}/paysages-tests

run_cli: all
	LD_LIBRARY_PATH=${BUILDPATH} ${BUILDPATH}/paysages-cli

run_qt: all
	LD_LIBRARY_PATH=${BUILDPATH} ${BUILDPATH}/paysages-qt

profile:
	sudo opcontrol --setup --no-vmlinux --event=CPU_CLK_UNHALTED:3000000:0:1:1 --buffer-size=65536 --callgraph=5 --separate=none
	sudo opcontrol --reset
	sudo opcontrol --start
	LD_LIBRARY_PATH=${BUILDPATH} ${BUILDPATH}/paysages-qt || true
	sudo opcontrol --shutdown
	opannotate --source --output-dir=annotated ${BUILDPATH}/libpaysages.so --base-dirs=. --search-dirs=lib_paysages
	opreport -l ${BUILDPATH}/libpaysages.so -c -g | less

install:release
	mkdir -p ${DESTDIR}/usr/bin
	mkdir -p ${DESTDIR}/usr/lib
	mkdir -p ${DESTDIR}/usr/share/paysages3d
	cp build/release/paysages-qt ${DESTDIR}/usr/bin/paysages3d
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
