CC = gcc                # Compiler to use
CFLAGS = -Wall -g       # Compilation flags

SERVER_SRC = server.c   # Server source file
CLIENT_SRC = client.c   # Client source file

SERVER_BIN = server     # Server executable
CLIENT_BIN = client1 client2 client3 client4  # Multiple client executables

# Default target
all: $(SERVER_BIN) $(CLIENT_BIN)

# Rule to compile the server
$(SERVER_BIN): $(SERVER_SRC)
	$(CC) $(CFLAGS) -o $(SERVER_BIN) $(SERVER_SRC)

# Rule to compile multiple clients
$(CLIENT_BIN): %: $(CLIENT_SRC)
	$(CC) $(CFLAGS) -o $@ $(CLIENT_SRC)

# Clean up the generated files
clean:
	rm -f $(SERVER_BIN) $(CLIENT_BIN)

# Rebuild everything from scratch
rebuild: clean all
