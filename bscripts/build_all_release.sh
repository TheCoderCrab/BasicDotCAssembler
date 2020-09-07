#!/bin/sh

./bscripts/build_module_release.sh    flat &&\
./bscripts/build_release.sh "$1" &&\
cp build/release/basic_dotc_assembler build/release/run/bdca