BUILDMODE=debug
BUILDPATH=./build/${BUILDMODE}

all:
	@+cd lib_paysages && make BUILDMODE=${BUILDMODE}
	@+cd cli && make BUILDMODE=${BUILDMODE}
	@+cd gui_qt && qmake "BUILDMODE=${BUILDMODE}" && make

clean:
	cd lib_paysages && make clean BUILDMODE=${BUILDMODE}
	cd cli && make clean BUILDMODE=${BUILDMODE}
	cd gui_qt && qmake "BUILDMODE=${BUILDMODE}" && make clean
	rm -f ${BUILDPATH}/paysages-qt
	rm -f ${BUILDPATH}/paysages-cli
	rm -f ${BUILDPATH}/libpaysages.so
	
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
