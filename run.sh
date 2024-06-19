#!/bin/bash
set -xe
cd src
make -j7
cd ..
./src/main ./input.earl
