BUILDMODE=debug
BUILDPATH=./build/${BUILDMODE}

all:
	@+cd src/rendering && make BUILDMODE=${BUILDMODE} PROJECT_PATH=${CURDIR}
	@+cd src/exploring && make BUILDMODE=${BUILDMODE} PROJECT_PATH=${CURDIR}
	@+cd src/controlling && make BUILDMODE=${BUILDMODE} PROJECT_PATH=${CURDIR}
	@+cd src/editing && qmake "BUILDMODE=${BUILDMODE}" "PROJECT_PATH=${CURDIR}" && make

clean:
	cd src/rendering && make clean BUILDMODE=${BUILDMODE} PROJECT_PATH=${CURDIR}
	cd src/exploring && make clean BUILDMODE=${BUILDMODE} PROJECT_PATH=${CURDIR}
	cd src/controlling && make clean BUILDMODE=${BUILDMODE} PROJECT_PATH=${CURDIR}
	cd src/editing && qmake "BUILDMODE=${BUILDMODE}" "PROJECT_PATH=${CURDIR}" && make clean
	rm -f ${BUILDPATH}/paysages-cli
	rm -f ${BUILDPATH}/paysages-qt
	rm -f ${BUILDPATH}/libpaysages_exploring.so
	rm -f ${BUILDPATH}/libpaysages_rendering.so

release:
	make BUILDMODE=release all

run_cli:
	LD_LIBRARY_PATH=${BUILDPATH} ${BUILDPATH}/paysages-cli

run_qt:
	LD_LIBRARY_PATH=${BUILDPATH} ${BUILDPATH}/paysages-qt

profile:
	sudo opcontrol --setup --no-vmlinux --event=CPU_CLK_UNHALTED:3000000:0:1:1 --buffer-size=65536 --callgraph=5 --separate=none
	sudo opcontrol --reset
	sudo opcontrol --start
	LD_LIBRARY_PATH=${BUILDPATH} ${BUILDPATH}/paysages-qt || true
	sudo opcontrol --shutdown
	opannotate --source --output-dir=annotated ${BUILDPATH}/libpaysages.so --base-dirs=. --search-dirs=lib_paysages
	opreport -l ${BUILDPATH}/libpaysages.so -c -g | less

.PHONY:all clean release
