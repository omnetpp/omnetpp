#!/bin/sh
DIR=`dirname $0`
nedfile=$1
if grep '^//--subclassing--' $nedfile >/dev/null; then 
    echo "     running: perl $DIR/msgc.pl $nedfile"
    perl $DIR/msgc.pl $nedfile
else
    echo "     running: $DIR/nedc $nedfile"
    $DIR/nedc $nedfile
fi