CC = gcc
CFLAGS = -fPIC -shared
DEBUG_FLAGS = -g -Wall -Werror -Wextra -static-libgcc -pedantic -ansi
SRC = asdf.c

target = libasdf
ifeq ($(OS),Windows_NT)  # Determine File Extension + OS-Specific Clean Command
	target := $(target).dll
	CLN := del $(target) 2>nul
else
	target := $(target).so
	CLN := rm -f $(target)
endif

ifeq ($(MAKECMDGOALS),debug) # Determine Appropriate Optimization Level
	CFLAGS += -Og
else
	CFLAGS += -O2
endif

.PHONY: all debug clean

all:
	$(CC) $(CFLAGS) -o $(target) $(SRC)

debug: CFLAGS += $(DEBUG_FLAGS)
debug: all

clean:
	@$(CLN)
