#!/bin/sh
# the next line restarts using wish \
exec wish "$0" "$@"

if [catch {file readlink $argv0} link] {set link ""}
set OMNETPP_PLOVE_DIR [file join [file dirname $argv0] [file dirname $link]]

source [file join $OMNETPP_PLOVE_DIR plove.tcl]

startPlove $argv

