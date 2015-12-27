#!/bin/bash

run () {
	local prog=$1
	local timeout=$2
	local inputfile=$3
	local PID=0
	local watchdog=0
	shift
	shift
	shift
	if [ x$timeout = x"" ]; then
		timeout=10
	fi

	t0=`date +%s.%N 2>/dev/null`
	$prog $@ <"$inputfile" &
	PID=$!
	(sleep $(($timeout)) && kill $PID) >/dev/null 2>&1 &
	watchdog=$!

	if wait $PID >/dev/null 2>&1; then
		kill $watchdog
	fi
}

# Usage: runtest <prog> [ -a <arg_num> <arg1> <arg2> ... ] [ -i <input> ] [ -t <timeout> ] [ -c <check fn> [ -a <arg_num> <arg1> <arg2> ... ] ]
runtest () {
	# process arguments
	local prog=$1
	local timeout=2
	local inputfile=""
	local checkfn=""
	shift
	declare -a progarg
	declare -a checkarg
	if [ x$1 = x"-a" ]; then
		shift
		local arg_num=$1
		shift
		for (( i = 0; i < $arg_num; i++ )); do
			progarg=("${progarg[@]}" $1);
			shift;
		done
	fi
	while [ $# -ne 0 ]; do
		case "$1" in
		"-t" )
			shift;
			timeout=$1;
			shift;
			;;
		"-i" )
			shift;
			inputfile=$1;
			shift;
			;;
		"-c" )
			shift;
			checkfn=$1;
			shift;
			if [ x$1 = x"-a" ]; then
				shift
				local arg_num=$1
				shift
				for (( i = 0; i < $arg_num; i++ )); do
					checkarg=("${checkarg[@]}" $1);
					shift;
				done
			fi
			;;
		* )
			echo "Unknown argument";
			return 255;
			;;
		esac
	done

	# run prog
	local output
	if [ "x$inputfile" != "x" ]; then
		output=`run $prog $timeout $inputfile ${progarg[@]} 2>&1 | tee a.out | while read line; do echo -n $line"\n"; done`
	else
		output=`run $prog $timeout $inputfile ${progarg[@]} /dev/null 2>&1 | while read line; do echo -n $line"\n"; done`
	fi

	# run check
	if [ x$checkfn != x ]; then
		local result=""
		result=$($checkfn "$output" ${checkarg[@]});
		retval=$?
		echo $result
		return $retval
	fi
}

checkregexps () {
	local content=$1
	shift
	local okay="yes"
	local not=false
	for i
	do
		if [ "x$i" = "x!" ]
		then
			not=true
		elif $not
		then
			echo -e $content | egrep "$i" >/dev/null
			if [ $? -eq 0 ]
			then
				echo "got unexpected line '$i'"
				okay=no
			fi
			not=false
		else
			echo -e $content | egrep "$i" >/dev/null
			if [ $? -ne 0 ]
			then
				echo "missing '$i'"
				okay=no
			fi
			not=false
		fi
	done
	if [ "$okay" = "yes" ]
	then
		return 0
	else
		return 1
	fi
}

checkfile () {
	echo -e $1 | diff -B - $2 | while read line
	do
		echo -n $line"\n"
	done;
	echo -e $1 | diff -B - $2 >/dev/null 2>&1
	return $?
}

# Usage: runtest <tagname> <defs> <check fn> <check args...>
runexpect () {
	perl -e "print '$1: '"
	rm -f obj/kern/init.o obj/kern/kernel obj/kern/kernel.img
	[ "$preservefs" = y ] || rm -f obj/fs/fs.img
	if $verbose
	then
		echo "$make $2... "
	fi
	$make $2 >$out
	if [ $? -ne 0 ]
	then
		rm -f obj/kern/init.o
		echo $make $2 failed
		exit 1
	fi
	# We just built a weird init.o that runs a specific test.  As
	# a result, 'make qemu' will run the last graded test and
	# 'make clean; make qemu' will run the user-specified
	# environment.  Remove our weird init.o to fix this.
	rm -f obj/kern/init.o
	# run
	# tail -f qemu.fifo | expect grade-breakpoint.sh &>qemu.fifo
	# expect -i  ./grade-breakpoint.sh
	./grade-breakpoint.sh

	if [ $? -ne 0 ]
	then
	    fail
	else
	    pass
	fi
}
