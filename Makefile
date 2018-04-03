#
# Předmět  :   IPP 2018
# Soubor   :   Makefile
# Projekt  :   Bandwidth Measurement
# Varianta :   1
# Autor    :   xkoval14, Marek Kovalčík
#

PROGRAM=ipk-mtrip
PROGRAMFILES=ipk-mtrip.c # případné další osubory
CFLAGS=-std=gnu99 -Wall -Wextra -pedantic -pthread #-Werror
CC=gcc
RM=rm -f

all : $(PROGRAM)
$(PROGRAM) : $(PROGRAMFILES)
		$(CC) $(CFLAGS) -o $(PROGRAM) $(PROGRAMFILES) -lm

clean :
	$(RM) *.o $(PROGRAM)