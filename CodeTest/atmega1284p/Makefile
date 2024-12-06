CMD=./robcmp-simavr-debug -c 16000000 -m atmega1284p -f 
CC=avr-gcc
ALL_TT=$(patsubst %.c,%.tt,$(wildcard *.c))
ALL_TIME=$(patsubst %.c,%.time,$(wildcard *.c))


all:out test

%.tt: %.c
	$(CC) -mmcu=atmega1284p $(OPT) $< -o out/$*
	@echo "\n"
	$(CMD) out/$*

test: ${ALL_TT}


time: ${ALL_TIME}

%.time : %.c
	$(CC) -mmcu=atmega1284p  $(OPT) $< -o out/$*
	@echo "\n"
	@-./timetest.sh $< out/$*

out: 
	mkdir -p out

size:
	avr-size out/*

clean:
	@rm out/*
	
