#!/bin/sh

export MAIN_DIR=$PWD

mkdir -p build/debug

cd build/debug

cmake -S $MAIN_DIR -B . -DDEBUG=1 -DGDB="$1"

make

export EXIT_CODE=$?

mkdir -p run
cp basic_dotc_assembler run/

exit $EXIT_CODE