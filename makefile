# Compiler and flags
CC = gcc
CFLAGS = -Wall -g

# Target executable
TARGET = test.out

# Include directories
INCLUDES = $(shell find . -type d -exec echo -I{} \;)

# Libraries
LIBRARIES = -lreadline

# Source files
SRCS = $(shell find . -name "*.c")

# Object files
OBJS = $(SRCS:.c=.o)

# Default target
all: $(TARGET)

# Link object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) -o $(TARGET) $(OBJS) $(LIBRARIES)

# Compile source files into object files
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDES)

# Clean up build files
clean:
	rm -f $(OBJS) $(TARGET)

debug:
	@echo "SRCS = $(SRCS)"
	@echo "OBJS = $(OBJS)"
	@echo "INCLUDES = $(INCLUDES)"
	@echo "LIBRARIES = $(LIBRARIES)"

run:
	./$(TARGET)

# Phony targets
.PHONY: all clean debug