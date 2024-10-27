# Define the compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -I./telibs

# Define the source and object files
LIB_SRC = telibs/telnet_lib.c
LIB_OBJ = telibs/telnet_lib.o
MAIN_SRC = main.c
MAIN_OBJ = main.o
EXEC = telnet_client

# Default target
all: $(EXEC)

# Link the executable
$(EXEC): $(MAIN_OBJ) $(LIB_OBJ)
	$(CC) -o $@ $^

# Compile the library source file
telibs/telnet.o: $(LIB_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

# Compile the main source file
$(MAIN_OBJ): $(MAIN_SRC)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up generated files
clean:
	rm -f $(EXEC) $(LIB_OBJ) $(MAIN_OBJ)

# Phony targets
.PHONY: all clean
