#!/bin/bash
CMD="./robcmp-simavr-debug -c 16000000 -m atmega328p -f $2"

GREEN="\033[0;32m"
RED="\033[0;31m"
NC="\033[0m"

echo -n "               $1"
make -s $2
if [ "$?" -eq 0 ]; then
	timeout 10s ${CMD} &> /dev/null
	#./$2 &> /dev/null
	if [ "$?" -eq 0 ]; then
		echo -e \\r${GREEN}[PASS]\\t\\t${NC}
	else
		echo -e \\r${RED}[FAILED]\\t${NC}
	fi
else
	echo -e \\r${RED}[BUILD FAILED]\\t${NC}
fi

