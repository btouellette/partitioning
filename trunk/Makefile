# Compiler to use
CC=g++
# Compiler flags
CFLAGS=-g -c -Wall -Wextra -O3
# Name of the output binary
BIN=part
# Include directory (*.h)
INCS=-I inc
# Source directory (*.cc)
SRCDIR=src
# Output directory (*.o)
OBJDIR=out
# Objects that need to be built
OBJS=$(OBJDIR)/main.o $(OBJDIR)/kl.o $(OBJDIR)/fm.o $(OBJDIR)/import.o $(OBJDIR)/graph.o $(OBJDIR)/annealing.o $(OBJDIR)/slicing.o

# Default all target builds single binary
all: $(BIN)

# Combine all the object files into the output binary
$(BIN): $(OBJS)
	$(CC) $(OBJS) -o $(BIN)

# For every cc file in src directory make an object file in the out dir
$(OBJDIR)/%.o : $(SRCDIR)/%.cc
	$(CC) $(CFLAGS) $(INCS) $< -o $@

# Run single KL run on test netlist but don't show the command
test: all
	@./part -in ISPD98/ibm01.netD -alg kl -runs 1

# Clean silently (@)
clean:
	@rm -rf out/* $(BIN)

# For when modifying headers
rebuild: clean all

# Disable checking for files named all or clean
.PHONY: all clean
