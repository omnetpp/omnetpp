#!/bin/sh
cd `dirname $0`
../src/${projectname} -n .:../src $*
# for shared lib, use: opp_run -l ../src/${projectname} -n .:../src $*
