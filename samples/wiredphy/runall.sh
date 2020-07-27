#!/bin/sh
sim=./wiredphy_dbg
for i in StandardEnd StandardEndWithPreemption StandardStart StandardStartWithPreemption StandardStartWithCutthrough StandardStartWithPreemptionAndCutthrough; do
	echo
	echo "*** $i"
	echo "$sim -c $i -s -u Cmdenv"
	$sim -c $i -s -u Cmdenv --sim-time-limit=10s >/dev/null
done
