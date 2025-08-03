#!/bin/bash
python generate_api.py wrapper/bullet_wrapper.cpp cbinding/bullet_api.h
rm -rf build
mkdir build && cd build
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
make -j$(nproc)
cd ..