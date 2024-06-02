#!/bin/bash

set -e

function usage() {
    echo "locks a file as read-only"
    echo "usage: $0 <filename>"
    exit 1
}

if [ "$#" != 1 ]; then
    usage $0
fi

chmod 444 $1
echo "$1 is now read-only"
