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
INCLUDE_DIRECTIVE="#include \"$TESTNAME/test-$TESTNAME.hpp\""

echo "TESTNAME: $TESTNAME"
echo "INCLUDE_DIRECTIVE: $INCLUDE_DIRECTIVE"

sed -i "s/test_suite_SOURCES = \(.*\)/test_suite_SOURCES = \1 ${TESTNAME}\/test-${TESTNAME}.cpp/" Makefile.am
echo "Update Makefile.am"

mkdir -v "$TESTNAME"

touch "$TESTNAME/test-$TESTNAME"{.cpp,.hpp}

echo "Created $TESTNAME/test-$TESTNAME"{.cpp,.hpp}

cat > "${TESTNAME}/test-${TESTNAME}.hpp" <<EOF
#ifndef ${TESTNAME^^}_H
#define ${TESTNAME^^}_H

#include "test-suite.hpp"

// Add test prototypes here

#endif // ${TESTNAME^^}_H
EOF

echo "Created prototype at ${TESTNAME}/test-${TESTNAME}.hpp"

echo -e "#include \"test-suite.hpp\"\n\n// Add test implementations here" > "$TESTNAME/test-$TESTNAME.cpp"

echo "Created implementation file at $TESTNAME/test-$TESTNAME.cpp"

sed -i "1i$INCLUDE_DIRECTIVE" test-suite.cpp

echo "Added necessary #include to test-suite.cpp"
echo "Done"
