#!/bin/bash
GREEN="\033[0;32m"
RED="\033[0;31m"
NC="\033[0m"

echo -n " ...           $1 "
make -s $2
REP=10
timer=0
avg=0
if [ "$?" -eq 0 ]; then
	timeout 10s ./$2 &> /dev/null
    STATUS=$?
    if [ "${STATUS}" -eq 0 ]; then

		TIMEFORMAT="%U" # get only the User time
		for i in $(seq 1 $REP); do
			timer=$timer+$({ time $CMD &>/dev/null; } 2>&1)
		done
		timer=$(echo "${timer//,/.}")
		avg=$(echo "scale=8;($timer)/$REP -q"| bc)
		
		echo -e \\r${GREEN}[PASS]\\t\\t${NC}\\t\\t\\t${avg}
	else
		echo -e \\r${RED}[FAILED] ${STATUS}\\t${NC}
	fi
else
	echo -e \\r${RED}[BUILD FAILED]\\t${NC}
fi

