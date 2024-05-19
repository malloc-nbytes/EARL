#!/bin/bash

set -e

function usage() {
    echo "usage: $0 <testname>"
    exit 1
}

if [ "$#" != 1 ]; then
    usage $0
fi

if [ "$1" = "--help" -o "$1" = "-h" ];
then
    usage $0
fi

TESTNAME=$1
INCLUDE_DIRECTIVE="#include \"$TESTNAME/test-$TESTNAME.h\""

echo "TESTNAME: $TESTNAME"
echo "INCLUDE_DIRECTIVE: $INCLUDE_DIRECTIVE"

sed -i "s/test_suite_SOURCES = \(.*\)/test_suite_SOURCES = \1 ${TESTNAME}\/test-${TESTNAME}.c/" Makefile.am
echo "Update Makefile.am"

mkdir -v "$TESTNAME"

touch "$TESTNAME/test-$TESTNAME"{.c,.h}

echo "Created $TESTNAME/test-$TESTNAME"{.c,.h}

cat > "${TESTNAME}/test-${TESTNAME}.h" <<EOF
#ifndef ${TESTNAME^^}_H
#define ${TESTNAME^^}_H

#include "test-suite.h"

// Add test prototypes here

#endif // ${TESTNAME^^}_H
EOF

echo "Created prototype at ${TESTNAME}/test-${TESTNAME}.h"

echo -e "#include \"test-suite.h\"\n\n// Add test implementations here" > "$TESTNAME/test-$TESTNAME.c"

echo "Created implementation file at $TESTNAME/test-$TESTNAME.c"

sed -i "1i$INCLUDE_DIRECTIVE" test-suite.c

echo "Added necessary #include to test-suite.c"
echo "Done"
