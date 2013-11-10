BUILDMODE=release
BUILDPATH=./build/${BUILDMODE}
LIBRARY_PATH=${BUILDPATH}/rendering:${BUILDPATH}/exploring:${BUILDPATH}/system:${BUILDPATH}/basics:${BUILDPATH}/definition:${BUILDPATH}/render/opengl:${BUILDPATH}/render/software:${BUILDPATH}/tests/googletest

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
	LD_LIBRARY_PATH=$(LIBRARY_PATH) ${BUILDPATH}/tests/paysages-tests

run_cli:build
	LD_LIBRARY_PATH=$(LIBRARY_PATH) ${RUNNER} ${BUILDPATH}/controlling/paysages-cli

run:build
	LD_LIBRARY_PATH=$(LIBRARY_PATH) ${RUNNER} ${BUILDPATH}/editing/paysages-gui

profile:debug
	LD_LIBRARY_PATH=build/debug/rendering:build/debug/exploring perf record -g fp ./build/debug/paysages-gui
	perf report -g

profile_cli:debug
	LD_LIBRARY_PATH=build/debug/rendering perf record -g fp ./build/debug/paysages-cli
	perf report -g

package:release
	rm -rf paysages3d-linux
	rm -f paysages3d-linux.tar.bz2
	mkdir paysages3d-linux
	mkdir paysages3d-linux/lib
	mkdir paysages3d-linux/cache
	cp $(BUILDPATH)/system/libpaysages_system.so* paysages3d-linux/lib/
	cp $(BUILDPATH)/basics/libpaysages_basics.so* paysages3d-linux/lib/
	cp $(BUILDPATH)/definition/libpaysages_definition.so* paysages3d-linux/lib/
	cp $(BUILDPATH)/render/software/libpaysages_render_software.so* paysages3d-linux/lib/
	cp $(BUILDPATH)/render/opengl/libpaysages_render_opengl.so* paysages3d-linux/lib/
	cp $(BUILDPATH)/rendering/libpaysages_rendering.so* paysages3d-linux/lib/
	cp $(BUILDPATH)/exploring/libpaysages_exploring.so* paysages3d-linux/lib/
	cp $(BUILDPATH)/editing/paysages-gui paysages3d-linux/lib/
	chmod +x paysages3d-linux/lib/paysages-gui
	cp -r data paysages3d-linux/
	cp -r cache/*.cache paysages3d-linux/cache/
	cp dist/paysages3d.sh paysages3d-linux/
	chmod +x paysages3d-linux/paysages3d.sh
	cp dist/collectlib.sh paysages3d-linux/
	chmod +x paysages3d-linux/collectlib.sh
	cd paysages3d-linux && ./collectlib.sh && rm collectlib.sh && cd -
	tar -cjvvf paysages3d-linux.tar.bz2  paysages3d-linux/

.PHONY:all clean release build
