#!/bin/bash

set -u

make -C simulator clean > /dev/null
make -C simulator PLATFORM=373 > /dev/null

SOFTWARE_PATH=platforms/373/software
make -C $SOFTWARE_PATH > /dev/null

function shell() {
	cat << EOF
Choose from the following test suites:

1) Just basic (runs only basic.s)
2) Samples (basic, blink, echo, echo_str, trivialS, trivialC, trivialPrintf)
3) All tests (/*.bin, /tests/*.bin)

You may skip this menu by providing an argument to this script:
	$0 OPT
EOF
	read -p 'Choose a test suite: ' suite
}

if [ $# -eq 1 ]; then
	suite=$1
else
	shell
fi

case $suite in 
	1)
		tests=$SOFTWARE_PATH/basic.bin
		;;
	2)
		tests=`ls $SOFTWARE_PATH/*.bin`
		tests="$tests `ls $SOFTWARE_PATH/tests/trivial*.bin`"
		;;
	3)
		tests=`ls $SOFTWARE_PATH/*.bin`
		tests="$tests `ls $SOFTWARE_PATH/tests/*.bin`"
		;;
	*)
		echo "ERR: $suite is not a valid test suite"
		exit 1
		;;
esac

echo "Running $(echo $tests | wc -w) tests..."
out=`tempfile`
err=`tempfile`
for t in $tests; do
	(sleep 1; echo -e 'yTestStr1\n' | nc -4 localhost 4100 > /dev/null) &
	#./simulator -f $t -r
	printf "%40s: " $t
	simulator/simulator -f $t -r > $out 2> $err
	ret=$?
	wait
	if [ $ret -eq 0 ]; then
		echo -e "\E[32mPASSED"; tput sgr0
	else
		echo -en "\E[31mFAILED"; tput sgr0; echo
		echo
		echo "Exited with return code $ret"
		echo "Simulator output left in $out"
		echo "Simulator errors left in $err"
		exit $ret
	fi
done
rm -f $out
rm -f $err
