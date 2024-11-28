CMD=./robcmp-simavr-debug -c 16000000 -m atmega328p -f a.out
CC=avr-gcc

all:
	clear

%.tt: %.c clean
	$(CC) -mmcu=atmega328p $(OPT) $<
	@echo "\n"
	$(CMD)

%.time : %.c clean
	$(CC) -mmcu=atmega328p $(OPT) $<
	@echo "\n"
	
	@-./timetest.sh $<

clean:
	@rm -f *.o *.out
