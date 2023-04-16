PROJECT_NAME       ?= paint-squares
PROJECT_SOURCES    ?= src/paint-squares.c src/canvas.c
PROJECT_EXECUTABLE ?= $(PROJECT_NAME)
PROJECT_REPO_LINK   = https://github.com/danielsource/paint-squares

CC             = gcc
LD             = ld
RM            ?= rm -f
CP            ?= cp
MKDIR         ?= mkdir
INCLUDE_PATHS ?=
LIB_PATHS     ?=
CPPFLAGS       = -D PROJECT_NAME=\"$(PROJECT_NAME)\" \
		 -D PROJECT_REPO_LINK=\"$(PROJECT_REPO_LINK)\"
CFLAGS         = -std=c99 -Wall -Wextra -Wpedantic -fPIC
LDFLAGS        =
LDLIBS         = -lraylib

objects = $(PROJECT_SOURCES:src/%.c=obj/%.o)

all: debug

release: CFLAGS += -O2
release: LDFLAGS += -s
release: $(PROJECT_EXECUTABLE)

debug: CPPFLAGS += -D DEBUG
debug: CFLAGS += -Og -g3
debug: $(PROJECT_EXECUTABLE)

clean:
	$(RM) $(PROJECT_EXECUTABLE) $(objects)

test: debug
	./paint-squares

$(PROJECT_EXECUTABLE): $(objects)
	$(CC) -o $@ $(objects) $(CFLAGS) $(LDFLAGS) $(LIB_PATHS) $(LDLIBS)

obj/%.o: src/%.c src/canvas.h src/paint-squares.h Makefile | obj
	$(CC) -c $< -o $@ $(CFLAGS) $(CPPFLAGS) $(INCLUDE_PATHS)

obj:
	$(MKDIR) $@

.PHONY: all release debug clean test
