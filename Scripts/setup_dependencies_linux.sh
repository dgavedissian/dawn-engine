#!/bin/bash

# Download the dependencies package
wget -O transcendent-deps.tar.gz https://dl.dropboxusercontent.com/u/1490400/transcendent/Dependencies/transcendent-deps.tar.gz
tar xzvf transcendent-deps.tar.gz
cd transcendent-deps/

# Invoke build script
./build-ubuntu.sh

# Move contents of the output to dependencies folder
rm -rf ../../dependencies/*/
mv unix/* ../../dependencies/

# Delete build dir to conserve disk space
cd ..
rm -rf transcendent-deps
