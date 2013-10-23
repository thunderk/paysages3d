#!/bin/bash

SCRIPT_PATH=$(dirname $0)
export LD_LIBRARY_PATH=${SCRIPT_PATH}/lib:"${LD_LIBRARY_PATH}"
cd ${SCRIPT_PATH}
COLLECT_NEEDED=$(ldd lib/paysages-gui | grep -c 'not found')
[ $COLLECT_NEEDED -gt 0 ] && export LD_LIBRARY_PATH=${SCRIPT_PATH}/lib/collected:"${LD_LIBRARY_PATH}"
${SCRIPT_PATH}/lib/paysages-gui
cd - > /dev/null

