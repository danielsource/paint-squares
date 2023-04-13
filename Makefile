PROJECT_NAME       ?= paint-squares
PROJECT_SOURCES    ?= paint-squares.c canvas.c
PROJECT_EXECUTABLE ?= $(PROJECT_NAME)
PROJECT_FONT       ?= assets/cascadia-code.ttf # NOTE: This font needs to be TTF.

CC            ?= gcc
LD            ?= ld
RM            ?= rm -f
CP            ?= cp
INCLUDE_PATHS ?=
LIB_PATHS     ?=
CPPFLAGS       = -D PROJECT_NAME=\"$(PROJECT_NAME)\"
CFLAGS         = -std=c99 -Wall -Wextra -Wpedantic -fPIC
LDFLAGS        =
LDLIBS         = -lraylib

objects = $(PROJECT_SOURCES:.c=.o) assets/font.o

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

assets/font.o:
	$(CP) $(PROJECT_FONT) $*
	$(LD) -r -b binary -o $@ $*; $(RM) $*

%.o: %.c assets.h canvas.h paint-squares.h Makefile
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS) $(INCLUDE_PATHS)

.PHONY: all release debug clean
