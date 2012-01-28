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

.PHONY:all clean release
