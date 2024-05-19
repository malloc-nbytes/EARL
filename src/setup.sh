#!/bin/bash

set -xe

autoreconf --install --verbose
./configure
make test
