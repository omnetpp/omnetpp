#!/bin/sh
DIR=`dirname $0`
cmdline=$*
shift `expr $# - 1`
nedfile=$1
if grep '^//--subclassing--' $nedfile >/dev/null; then 
    echo "     running: opp_msgc $nedfile"
    perl $DIR/opp_msgc $nedfile
else
    echo "     running: $DIR/nedc $cmdline"
    $DIR/nedc $cmdline
fi
