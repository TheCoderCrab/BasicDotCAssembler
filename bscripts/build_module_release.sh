#!/bin/sh

MAIN_DIR=$PWD

cd plateforms/$1

mkdir -p $MAIN_DIR/build/plateforms/$1

cmake -S . -B $MAIN_DIR/build/plateforms/$1

mkdir -p $MAIN_DIR/build/release/run/modules/$1/

cp -r module_folder/* $MAIN_DIR/build/release/run/modules/$1/

cd $MAIN_DIR/build/plateforms/$1

make

cp lib$1.so $MAIN_DIR/build/release/run/modules/$1/$1.linux.so