#!/bin/sh

# This script will build in release all modules then put everything in a tar package

./bscripts/clean.sh

mkdir -p build/release/run

./bscripts/build_all_release.sh

cp ./bscripts/install.sh build/release/run
cp ./bscripts/install_modules.sh build/release/run
cp ./bscripts/uninstall.sh build/release/run

mkdir -p release

MAIN_DIR=$PWD

cd build/release/run && tar -czvf $MAIN_DIR/release/v$1-linux.tar.gz *