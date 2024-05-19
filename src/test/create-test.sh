#!/bin/bash

set -e

function usage() {
    echo "usage: $0 <testname>"
    exit 1
}

if [ "$#" != 1 ]; then
    usage $0
fi

TESTNAME=$1
INCLUDE_DIRECTIVE="#include \"$TESTNAME/test-$TESTNAME.h\""

sed -i "s/test_suite_SOURCES = \(.*\)/test_suite_SOURCES = \1 ${TESTNAME}\/test-${TESTNAME}.c/" Makefile.am

mkdir "$TESTNAME"

touch "$TESTNAME/test-$TESTNAME"{.c,.h}

cat > "${TESTNAME}/test-${TESTNAME}.h" <<EOF
#ifndef ${TESTNAME^^}_H
#define ${TESTNAME^^}_H

#include "test-suite.h"

// Add test prototypes here

#endif // ${TESTNAME^^}_H
EOF

echo -e "#include \"test-suite.h\"\n\n// Add test implementations here" > "$TESTNAME/test-$TESTNAME.c"

sed -i "1i$INCLUDE_DIRECTIVE" test-suite.c
