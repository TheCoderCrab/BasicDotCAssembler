#!/bin/sh

export MAIN_DIR=$PWD

mkdir -p build/release

cd build/release

cmake -S $MAIN_DIR -B .

make

export EXIT_CODE=$?

mkdir -p run
cp basic_dotc_assembler run/

exit $EXIT_CODE