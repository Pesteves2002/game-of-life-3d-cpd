

CC ?= g++
LD ?= g++

SOURCES := $(wildcard */*.cpp)
SOURCES += $(wildcard */*.c)
HEADERS := $(wildcard */*.h)
OBJECTS := $(SOURCES:.cpp=.o)
OBJECTS += $(SOURCES:.c=.o)
TARGET_EXEC ?= life3d

CFLAGS = -std=c++17

# Warnings
CFLAGS += -fdiagnostics-color=always -Wall -Werror -Wextra -Wcast-align -Wconversion -Wfloat-equal -Wformat=2 -Wnull-dereference -Wshadow -Wsign-conversion -Wswitch-default -Wswitch-enum -Wundef -Wunreachable-code -Wunused
# Warning suppressions
CFLAGS += -Wno-sign-compare

LDFLAGS = -fopenmp

.phony: all clean

all: $(TARGET_EXEC)

clean:
	rm -f $(OBJECTS) $(TARGET_EXEC)

