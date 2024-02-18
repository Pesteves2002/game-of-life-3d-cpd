CC ?= g++
LD ?= g++

INCLUDE_DIRS := .
INCLUDES = $(addprefix -I,$(INCLUDE_DIRS))

SOURCES := $(wildcard *.cpp)
SOURCES += $(wildcard */*.cpp)
HEADERS := $(wildcard *.h)
HEADERS += $(wildcard */*.h)
OBJECTS  := $(SOURCES:.cpp=.o)
TARGET_EXECS := life3d
TARGET_EXECS += tests/read tests/write tests/state

CFLAGS := -std=c++17 
CFLAGS += $(INCLUDES)
LDFLAGS := -fopenmp

# VPATH is a variable used by Makefile which finds *sources* and makes them available throughout the codebase
# vpath %.h <DIR> tells make to look for header files in <DIR>
vpath # clears VPATH
vpath %.h $(INCLUDE_DIRS)

# warnings
CFLAGS += -fdiagnostics-color=always -Wall -Werror -Wextra -Wcast-align -Wfloat-equal -Wformat=2 -Wnull-dereference -Wshadow -Wswitch-default -Wswitch-enum -Wundef -Wunreachable-code -Wunused
# warning suppressions
CFLAGS += -Wno-sign-compare

CFLAGS += -O3 -g

# conversion warnings
# CFLAGS += -Wconversion -Wsign-conversion

.PHONY: all clean fmt

all: $(TARGET_EXECS)

life3d: utils.o simulation.o

tests/read:  utils.o simulation.o
tests/write: utils.o simulation.o
tests/state: utils.o simulation.o

clean:
	rm -f $(OBJECTS) $(TARGET_EXECS)

fmt:
	clang-format -i $(SOURCES) $(HEADERS)
