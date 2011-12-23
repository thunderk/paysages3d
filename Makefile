all:
	cd lib_paysages && make
	cd gui_gtk && make

clean:
	cd lib_paysages && make clean
	cd gui_gtk && make clean

.PHONY:all clean
