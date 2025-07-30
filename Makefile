# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Directories
SRCDIR = src
BUILDDIR = build
TARGET = alba-compiler

# Find all .c files in the source directory
SOURCES = $(wildcard $(SRCDIR)/*.c)
# Create object file names by replacing src/ with build/ and .c with .o
OBJECTS = $(patsubst $(SRCDIR)/%.c, $(BUILDDIR)/%.o, $(SOURCES))

# The default target to build when you run "make"
all: $(TARGET)

# Rule to link the final executable
$(TARGET): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJECTS)

# Rule to compile a .c file into a .o object file
$(BUILDDIR)/%.o: $(SRCDIR)/%.c
	@mkdir -p $(BUILDDIR) # Create build directory if it doesn't exist
	$(CC) $(CFLAGS) -c $< -o $@

# Target to clean up the project (remove compiled files)
clean:
	rm -rf $(BUILDDIR) $(TARGET)

# Target to run the compiler on our example
run: all
	./$(TARGET) examples/hello.al

.PHONY: all clean run