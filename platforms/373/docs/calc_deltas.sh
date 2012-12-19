#!/bin/bash

function usage() {
cat << EOF
USAGE: $0 PROG1 PROG2

Calculates instruction overlap. Runs the simulator with -p flag
to do this, so must not be in student build and must have external
input for UART test cases

Requires PROG to be a full path to bin (e.g. programs/blink.bin)

If PROG does not exist, the script will attempt to 'make PROG'
EOF
popd
exit
}

pushd ../

if [ -z "$1" ] || [ -z "$2" ]; then
	usage
fi

if [ ! -e $1 ]; then
	make $1
fi
if [ ! -e $1 ]; then
	echo "ERR: Could not find or make $1"
	popd
	exit 1
fi

if [ ! -e $2 ]; then
	make $2
fi
if [ ! -e $2 ]; then
	echo "ERR: Could not find or make $2"
	popd
	exit 1
fi

if [ ! -x simulator ]; then
	make simulator
fi
if [ ! -x simulator ]; then
	echo "ERR: Could not find or make simulator binary"
	popd
	exit 1
fi

inst1=`tempfile`
inst2=`tempfile`

./simulator -f $1 -p | grep '   P' | awk '{print $7}' | sort | uniq > $inst1
./simulator -f $2 -p | grep '   P' | awk '{print $7}' | sort | uniq > $inst2

echo "Instructions in both $1 and $2:"
comm -12 $inst1 $inst2
echo "`comm -12 $inst1 $inst2|wc -l` total"
echo "Instructions only in $1:"
comm -23 $inst1 $inst2
echo "`comm -23 $inst1 $inst2|wc -l` total"
echo "Instructions only in $2:"
comm -13 $inst1 $inst2
echo "`comm -13 $inst1 $inst2|wc -l` total"

rm -f $inst1
rm -f $inst2

popd
