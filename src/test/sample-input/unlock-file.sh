#!/bin/bash

set -e

function usage() {
    echo "unlocks a file to make it writable"
    echo "usage: $0 <filename>"
    exit 1
}

if [ "$#" != 1 ]; then
    usage $0
fi

chmod 600 $1
echo "$1 is now writable"
