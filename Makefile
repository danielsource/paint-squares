PROJECT_NAME       ?= paint-squares
PROJECT_SOURCES    ?= paint-squares.c
PROJECT_EXECUTABLE ?= $(PROJECT_NAME)
PROJECT_FONT_MONOSPACE ?= assets/cascadia-code.ttf

CC            ?= gcc
LD            ?= ld
RM            ?= rm -f
CP            ?= cp
INCLUDE_PATHS ?=
LIB_PATHS     ?=
CPPFLAGS       = -D PROJECT_NAME=\"$(PROJECT_NAME)\"
CFLAGS         = -std=c99 -Wall -Wextra -Wpedantic -fPIC
LDFLAGS        =
LDLIBS         = -lraylib -lm

objects = $(PROJECT_SOURCES:.c=.o) assets/font-monospace.o

all: debug

release: CFLAGS += -O2
release: LDFLAGS += -s
release: $(PROJECT_EXECUTABLE)

debug: CPPFLAGS += -D DEBUG
debug: CFLAGS += -Og -g3
debug: $(PROJECT_EXECUTABLE)

clean:
	$(RM) $(PROJECT_EXECUTABLE) $(objects)

$(PROJECT_EXECUTABLE): $(objects)
	$(CC) -o $@ $(objects) $(CFLAGS) $(LDFLAGS) $(LIB_PATHS) $(LDLIBS)

# HACK: Create font object to embed in executable.
assets/font-monospace.o:
	$(CP) $(PROJECT_FONT_MONOSPACE) $*
	$(LD) -r -b binary -o $@ $*; $(RM) $*

%.o: %.c assets.h Makefile
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS) $(INCLUDE_PATHS)

.PHONY: all release debug clean
