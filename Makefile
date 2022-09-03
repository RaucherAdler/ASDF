CC = gcc
CFLAGS = -fPIC -shared
DEBUG_FLAGS = -g -Wall -Werror -Wextra -static-libgcc -pedantic -ansi
SRC = asdf.c

target = libasdf
ifeq ($(OS),Windows_NT)  # Determine File Extension + OS-Specific Clean Command
	target := $(target).dll
	CLN := del
else
	target := $(target).so
	CLN := rm -f
endif

ifneq ($(findstring debug,$(MAKECMDGOALS)),) # Determine Appropriate Optimization Level
	CFLAGS += $(DEBUG_FLAGS)
else
	CFLAGS += -O2
endif

.PHONY: all debug clean

all:
	$(CC) $(CFLAGS) -o $(target) $(SRC)

debug: CFLAGS += $(DEBUG_FLAGS)
debug: all

clean:
	$(CLN) $(target)
