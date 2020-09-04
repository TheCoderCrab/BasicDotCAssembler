#!/bin/sh

echo $DOTCVM_SRC_PATH

./bscripts/build_all_debug.sh &&\

cd build/debug/run && ./basic_dotc_assembler $@ &&\
cp out.dce $DOTCVM_SRC_PATH/build/debug/run/files/bios.bin