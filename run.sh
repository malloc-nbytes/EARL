#!/bin/bash
set -xe
cd src
make -j12
cd ..
./src/main ./input.earl
