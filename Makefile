all:
	@+cd lib_paysages && make
	@+cd cli && make
	@+cd gui_qt && qmake && make

clean:
	cd lib_paysages && make clean
	cd cli && make clean
	cd gui_qt && make clean && rm -f paysages-qt

run_cli:
	LD_LIBRARY_PATH=lib_paysages ./cli/paysages-cli

run_qt:
	LD_LIBRARY_PATH=lib_paysages ./gui_qt/paysages-qt

.PHONY:all clean
