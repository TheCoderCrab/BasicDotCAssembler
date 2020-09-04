#!/bin/sh

./bscripts/build_module_debug.sh    flat &&\
./bscripts/build_debug.sh "$1" &&\
cp build/debug/basic_dotc_assembler build/debug/run