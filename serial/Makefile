CC = g++
LD = g++

INCLUDE_DIRS := .
INCLUDES = $(addprefix -I, $(INCLUDE_DIRS))

SOURCES := $(wildcard *.cpp)
SOURCES += $(wildcard */*.cpp)
HEADERS := $(wildcard *.h)
HEADERS += $(wildcard */*.h)
OBJECTS  := $(SOURCES:.cpp=.o)
TARGET_EXECS := life3d
TARGET_EXECS += tests/read tests/write tests/state tests/example

# VPATH is a variable used by Makefile which finds *sources* and makes them available throughout the codebase
# vpath %.h <DIR> tells make to look for header files in <DIR>
vpath %.h $(INCLUDE_DIRS)

CXXFLAGS := -fopenmp
LDFLAGS := -fopenmp

CXXFLAGS := -Wall -Werror -Wextra -Wcast-align -Wfloat-equal -Wformat=2 -Wnull-dereference -Wshadow -Wswitch-default -Wswitch-enum -Wundef -Wunreachable-code -Wunused
# warning suppressions
CXXFLAGS += -Wno-sign-compare

#CXXFLAGS += -g
CXXFLAGS += -O3 -std=c++17
CXXFLAGS += $(INCLUDES)

# conversion warnings
# CXXFLAGS += -Wconversion -Wsign-conversion

.PHONY: all clean fmt test

all: $(TARGET_EXECS)

life3d: utils.o simulation.o leaderboard.o

tests/read:  utils.o simulation.o leaderboard.o
tests/write: utils.o simulation.o leaderboard.o
tests/state: utils.o simulation.o leaderboard.o
tests/example: utils.o simulation.o leaderboard.o

clean:
	rm -f $(OBJECTS) $(TARGET_EXECS)

fmt:
	clang-format -i $(SOURCES) $(HEADERS)

test: $(TARGET_EXECS)
	for x in $(TARGET_EXECS); do \
		[ "$$x" != "life3d" ] &&\
		./$$x; \
	done

