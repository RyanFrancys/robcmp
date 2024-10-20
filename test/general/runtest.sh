#!/bin/bash
GREEN="\033[0;32m"
RED="\033[0;31m"
NC="\033[0m"

echo -n " ...           $1 "
make -s $2
if [ "$?" -eq 0 ]; then
	timeout 10s ./$2 &> /dev/null
    STATUS=$?
    if [ "${STATUS}" -eq 0 ]; then
		echo -e \\r${GREEN}[PASS]\\t\\t${NC}
	else
		echo -e \\r${RED}[FAILED] ${STATUS}\\t${NC}
	fi
else
	echo -e \\r${RED}[BUILD FAILED]\\t${NC}
fi

