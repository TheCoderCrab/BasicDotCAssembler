#!/bin/sh

./bscripts/build_all_release.sh &&\

cd build/release/run && ./bdca $@
