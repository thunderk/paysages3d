BUILDMODE=release
BUILDPATH=./build/${BUILDMODE}
LIBRARY_PATH=${BUILDPATH}/system:${BUILDPATH}/basics:${BUILDPATH}/definition:${BUILDPATH}/render/software:${BUILDPATH}/render/preview:${BUILDPATH}/render/opengl:${BUILDPATH}/tests/googletest
BUILD_SPEC=linux-g++

all:build

dirs:
	mkdir -p ${BUILDPATH}

makefiles:dirs
ifeq (${BUILDMODE}, release)
	@+cd ${BUILDPATH} && qmake ../../src/paysages.pro -r -spec $(BUILD_SPEC)
else
	@+cd ${BUILDPATH} && qmake ../../src/paysages.pro -r -spec $(BUILD_SPEC) CONFIG+=debug CONFIG+=declarative_debug CONFIG+=qml_debug
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
ifdef TESTCASE
	LD_LIBRARY_PATH=$(LIBRARY_PATH) ${BUILDPATH}/tests/paysages-tests --gtest_filter=$(TESTCASE).*
else
	LD_LIBRARY_PATH=$(LIBRARY_PATH) ${BUILDPATH}/tests/paysages-tests
endif

run_cli:build
	LD_LIBRARY_PATH=$(LIBRARY_PATH) ${RUNNER} ${BUILDPATH}/interface/commandline/paysages-cli

run:build
	LD_LIBRARY_PATH=$(LIBRARY_PATH) ${RUNNER} ${BUILDPATH}/interface/desktop/paysages-gui

profile:build
	LD_LIBRARY_PATH=${LIBRARY_PATH} perf record -g fp ${BUILDPATH}/interface/desktop/paysages-gui
	perf report -g

profile_cli:build
	LD_LIBRARY_PATH=${LIBRARY_PATH} perf record -g fp ${BUILDPATH}/interface/commandline/paysages-cli
	perf report -g

package:build
	rm -rf paysages3d-linux
	rm -f paysages3d-linux.tar.bz2
	mkdir paysages3d-linux
	mkdir paysages3d-linux/lib
	mkdir paysages3d-linux/cache
	cp $(BUILDPATH)/system/libpaysages_system.so* paysages3d-linux/lib/
	cp $(BUILDPATH)/basics/libpaysages_basics.so* paysages3d-linux/lib/
	cp $(BUILDPATH)/definition/libpaysages_definition.so* paysages3d-linux/lib/
	cp $(BUILDPATH)/render/software/libpaysages_render_software.so* paysages3d-linux/lib/
	cp $(BUILDPATH)/render/preview/libpaysages_render_software.so* paysages3d-linux/lib/
	cp $(BUILDPATH)/render/opengl/libpaysages_render_opengl.so* paysages3d-linux/lib/
	cp $(BUILDPATH)/interface/desktop/paysages-gui paysages3d-linux/lib/
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
