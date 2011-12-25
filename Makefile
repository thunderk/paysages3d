all:
	cd lib_paysages && make
	cd gui_gtk && make
	cd gui_qt && qmake && make

clean:
	cd lib_paysages && make clean
	cd gui_gtk && make clean
	cd gui_qt && make clean && rm -f Makefile paysages-qt

run_gtk:
	LD_LIBRARY_PATH=lib_paysages ./gui_gtk/paysages-gtk

run_qt:
	LD_LIBRARY_PATH=lib_paysages ./gui_qt/paysages-qt

.PHONY:all clean
