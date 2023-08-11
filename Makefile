CC = gcc
DEBUG_FLAGS = -g3 -Wall -Werror -Wextra -static-libgcc -pedantic -ansi
LD_FLAGS = -fPIC -shared -Wl,-soname,$(soname)
 
SRC = asdf.c

PREFIX = /usr

MAJ = 1
MIN = 0
PATCH = 0
target = libasdf
ifeq ($(OS),Windows_NT)  # Determine File Extension + OS-Specific Clean Command
	target := $(target).dll
	CLN := del
else
	target := $(target).so
	CLN := rm -f
endif
soname = $(target).$(MAJ)
realname = $(target).$(MAJ).$(MIN).$(PATCH)



ifneq ($(findstring debug,$(MAKECMDGOALS)),) # Determine Appropriate Optimization Level
	CFLAGS += $(DEBUG_FLAGS)
else
	CFLAGS += -O2
endif

.PHONY: all debug clean install uninstall

all:
	$(CC) $(CFLAGS) $(LD_FLAGS) -o $(realname) $(SRC)

debug: CFLAGS += $(DEBUG_FLAGS)
debug: all

install: all
	mkdir -p $(PREFIX)/lib
	cp -f $(realname) $(PREFIX)/lib
	ldconfig -n $(PREFIX)/lib
	ln -sf $(PREFIX)/lib/$(realname) $(PREFIX)/lib/$(target)
	mkdir -p $(PREFIX)/include/asdf
	cp -f asdf.h $(PREFIX)/include/asdf

uninstall:
	rm -f $(PREFIX)/lib/$(realname)
	rm -f $(PREFIX)/lib/$(target)
	ldconfig -n $(prefix)/lib
	rm -rf $(PREFIX)/include/asdf

clean:
	$(CLN) $(target).*
