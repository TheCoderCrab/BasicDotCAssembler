#!/bin/sh

./bscripts/build_all_debug.sh &&\

cd build/debug/run && ./basic_dotc_assembler $@ &&\
