CC = mpicc
LD = mpicc

INCLUDE_DIRS := .
INCLUDES = $(addprefix -I, $(INCLUDE_DIRS))

SOURCES := $(wildcard *.c)
SOURCES += $(wildcard */*.c)
HEADERS := $(wildcard *.h)
HEADERS += $(wildcard */*.h)
OBJECTS  := $(SOURCES:.c=.o)
TARGET_EXECS := life3d-mpi
#TARGET_EXECS += tests/read tests/write tests/state tests/example

# VPATH is a variable used by Makefile which finds *sources* and makes them available throughout the codebase
# vpath %.h <DIR> tells make to look for header files in <DIR>
vpath %.h $(INCLUDE_DIRS)

CFLAGS := -fopenmp
LDFLAGS := -fopenmp

#CFLAGS := -Wall  -Wextra -Wcast-align -Wfloat-equal -Wformat=2 -Wnull-dereference -Wshadow -Wswitch-default -Wswitch-enum -Wundef -Wunreachable-code -Wunused
# warning suppressions
#CFLAGS += -Wno-sign-compare

#CFLAGS += -g
CFLAGS += -O2 -std=c17
CFLAGS += $(INCLUDES)

# conversion warnings
# CFLAGS += -Wconversion -Wsign-conversion

.PHONY: all clean fmt test

all: $(TARGET_EXECS)

life3d-mpi: utils.o simulation.o

tests/read:  utils.o simulation.o .o
tests/write: utils.o simulation.o .o
tests/state: utils.o simulation.o .o
tests/example: utils.o simulation.o .o

clean:
	rm -f $(OBJECTS) $(TARGET_EXECS)

fmt:
	clang-format -i $(SOURCES) $(HEADERS)

test: $(TARGET_EXECS)
	for x in $(TARGET_EXECS); do \
		[ "$$x" != "life3d" ] &&\
		./$$x; \
	done

