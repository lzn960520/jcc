#!/bin/bash

. ./grade-functions.sh

prog=$1
for i in `find -name $1"-test*.in" | sort`; do
	echo -n "[TEST] $i -> $prog ..."
	failmsg=$(runtest ./$1 "-i" $i "-c" "checkfile" "-a" 1 `echo $i | sed "s/\.in$/\.out/"`)
	if [ $? -ne 0 ]; then
		echo "fail"
		echo -e $failmsg
	else
		echo "pass"
	fi
done
