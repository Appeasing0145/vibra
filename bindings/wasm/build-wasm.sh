#!/bin/bash

# build libvibra as wasm
cd ..
rm -rf build
mkdir -p build
cd build || exit 1

emcmake cmake ../.. \
    -DLIBRARY_ONLY=ON \
    -DBUILD_TESTING=OFF \
    -DCMAKE_INSTALL_PREFIX=/usr/local

make -j
sudo make install

# build the wasm module
cd ../wasm || exit 1
rm -rf build 
mkdir -p build
emmake make
