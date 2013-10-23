#!/bin/bash

mkdir lib/collected
for lib in `ldd lib/paysages-gui lib/libpaysages_*.so | grep '/qt/' | cut -f 2 | cut -f 3 -d ' ' | sort | uniq`
do
    cp ${lib} lib/collected
    qtpath=$(dirname ${lib})
done
mkdir lib/platforms
cp ${qtpath}/../plugins/platforms/libqxcb.so lib/platforms

