# Compiler to use
CC=gcc
# Compiler flags
CFLAGS=-g -c -Wall -Wextra -O3 -std=c99
LDFLAGS=-lm
# Name of the output binary
BIN=floor
# Include directory (*.h)
INCS=-I inc
# Source directory (*.cc)
SRCDIR=src
# Output directory (*.o)
OBJDIR=out
# Objects that need to be built
OBJS=$(OBJDIR)/floor.o

# Default all target builds single binary
all: $(BIN)

# Combine all the object files into the output binary
$(BIN): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) -o $(BIN)

# For every c file in src directory make an object file in the out dir
$(OBJDIR)/%.o : $(SRCDIR)/%.c
	$(CC) $(CFLAGS) $(INCS) $< -o $@

# Run single KL run on test netlist but don't show the command
test: all
	@./floor -r 1 -i floor_bench/m10.blk -o test.out -d 0

# Clean silently (@)
clean:
	@rm -rf out/* $(BIN)

# For when modifying headers
rebuild: clean all

# Disable checking for files named all or clean
.PHONY: all clean
