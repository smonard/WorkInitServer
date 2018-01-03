#!/bin/sh

rm -rf build/
rm -rf deploy/
mkdir deploy
mkdir build/ && cd build/
cmake ..
cmake --build .
cd ../deploy
cp ../build/work_init_s_run ./
cp ../key.pem ./
cp ../cert.pem ./
./work_init_s_run

