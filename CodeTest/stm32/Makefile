CMD=./robcmp-simavr-debug -c 16000000 -m atmega328p -f 
CC=arm-nome-eabi-gcc
ALL_TT=$(patsubst %.c,%.tt,$(wildcard *.c))


all:out test

%.tt: %.c
	$(CC) -mmcu=atmega328p $(OPT) $< -o out/$*
	@echo "\n"
	$(CMD) out/$*

test: ${ALL_TT}

%.t : %.c
	$(CC) -mmcu=atmega328p $(OPT) $< -o out/$*
	$(CMD) out/$* -g

out: 
	@mkdir -p out

size:
	@avr-size out/*

clean:
	@rm out/*
	
