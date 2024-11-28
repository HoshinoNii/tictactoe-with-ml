# Compiler and flags
CC_X64 = gcc
CC_ARM = cc
CFLAGS = -I../../raylib/src

# Platform detection
ifeq ($(OS),Windows_NT)
    # Windows-specific settings
    CC = $(CC_X64)
    INCLUDES =
    LDFLAGS = -L../../raylib/src
    LIBS = -lraylib -lm -lpthread -lopengl32 -lgdi32 -lwinmm
else
    # Check for ARM architecture
    UNAME_M := $(shell uname -m)
    ifneq (,$(filter $(UNAME_M),armv7l aarch64))
        # ARM-specific settings
        CC = $(CC_ARM)
        INCLUDES = -I/opt/vc/include
        LDFLAGS = core/checkWinner.s -L../../raylib/src -L/opt/vc/lib
        LIBS = -lraylib -lm -lpthread -lGLESv2 -lEGL -lvchiq_arm -lbcm_host
    else
        # Default to x64 settings for other Unix-like systems
        CC = $(CC_X64)
        INCLUDES =
        LDFLAGS = -L../../raylib/src
        LIBS = -lraylib -lm -lpthread
    endif
endif

# Source files and target
SRC = main.c
TARGET = build/game

.PHONY: all
all:
	$(CC) -o $(TARGET) $(SRC) $(CFLAGS) $(INCLUDES) $(LDFLAGS) $(LIBS)
