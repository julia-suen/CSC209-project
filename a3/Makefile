# Simple Makefile for chat client and server

# Compile server
server: src/server.c src/serverutil.c src/common.c src/protocol.c
	gcc -Wall -Wextra -g -Isrc/include src/server.c src/serverutil.c src/common.c src/protocol.c -o server

# Compile client
client: src/client.c src/common.c src/protocol.c
	gcc -Wall -Wextra -g -Isrc/include src/client.c src/common.c src/protocol.c src/ui.c src/commands.c -o client

# Clean compiled executables
clean:
	rm -f server client

