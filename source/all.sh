#!/bin/bash
HEADER_NAME="all.hpp"

rm -f $HEADER_NAME

HEADER_LIST=`ls -1 *.hpp`

echo "#ifndef _LIBICT_HEADER" >> $HEADER_NAME
echo "#define _LIBICT_HEADER" >> $HEADER_NAME

for HEADER in $HEADER_LIST
do
    if [[ "$HEADER" != "test.hpp" && "$HEADER" != "git_version.h" ]]; then
        echo "#include \"$HEADER\"" >> $HEADER_NAME
    fi
done

echo "#endif" >> $HEADER_NAME
