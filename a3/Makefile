PORT = 58086
FLAGS = -Wall -Wextra -g -DPORT=${PORT}

.PHONY: all clean setup

all: setup bin/server bin/client

setup:
	-mkdir bin
	-mkdir obj

bin/server: obj/server.o obj/serverutil.o obj/protocol.o obj/common.o
	gcc ${FLAGS} -o $@ $^

bin/client: obj/client.o obj/ui.o obj/protocol.o obj/commands.o obj/common.o
	gcc ${FLAGS} -o $@ $^

# Header dependencies
obj/server.o : include/protocol.h include/server.h include/serverutil.h include/common.h
obj/serverutil.o : include/protocol.h include/server.h
obj/client.o : include/protocol.h include/commands.h include/ui.h
obj/ui.o : include/ui.h
obj/commands.o : include/commands.h include/common.h
obj/protocol.o : include/protocol.h
obj/common.o : include/common.h

obj/%.o: src/%.c
	gcc ${FLAGS} -c $< -o $@

clean:
	rm -rf bin obj