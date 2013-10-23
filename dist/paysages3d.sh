#!/bin/sh

SCRIPT_PATH=$(dirname $0)
cd ${SCRIPT_PATH}
LD_LIBRARY_PATH=${SCRIPT_PATH}/lib ${SCRIPT_PATH}/lib/paysages-gui
cd -

