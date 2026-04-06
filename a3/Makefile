.PHONY: all clean setup

all: setup bin/server bin/client

# This runs first and creates the folders. 
# The '-' means "keep going even if the folder already exists"
setup:
	-mkdir bin
	-mkdir obj

# Linking the Server
bin/server: obj/server.o obj/serverutil.o obj/protocol.o obj/common.o
	gcc -Wall -g -o bin/server obj/server.o obj/serverutil.o obj/protocol.o obj/common.o

# Linking the Client
bin/client: obj/client.o obj/ui.o obj/protocol.o obj/commands.o obj/common.o
	gcc -Wall -g -o bin/client obj/client.o obj/ui.o obj/protocol.o obj/commands.o obj/common.o

# Compiling Object Files
obj/server.o: src/server.c include/protocol.h include/server.h include/serverutil.h include/common.h
	gcc -Wall -g -c src/server.c -o obj/server.o

obj/serverutil.o: src/serverutil.c include/protocol.h include/server.h
	gcc -Wall -g -c src/serverutil.c -o obj/serverutil.o

obj/client.o: src/client.c include/protocol.h include/commands.h include/ui.h
	gcc -Wall -g -c src/client.c -o obj/client.o

obj/ui.o: src/ui.c include/ui.h
	gcc -Wall -g -c src/ui.c -o obj/ui.o

obj/commands.o: src/commands.c include/commands.h include/common.h
	gcc -Wall -g -c src/commands.c -o obj/commands.o

obj/protocol.o: src/protocol.c include/protocol.h
	gcc -Wall -g -c src/protocol.c -o obj/protocol.o

obj/common.o: src/common.c include/common.h
	gcc -Wall -g -c src/common.c -o obj/common.o

clean:
	rm -rf obj bin