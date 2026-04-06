.PHONY: all clean

all: bin/server bin/client


bin/server: obj/server.o obj/serverutil.o obj/protocol.o obj/common.o
	mkdir bin
	gcc -Wall -g -o bin/server obj/server.o obj/serverutil.o obj/protocol.o obj/common.o

bin/client: obj/client.o obj/ui.o obj/protocol.o obj/commands.o obj/common.o
	mkdir bin
	gcc -Wall -g -o bin/client obj/client.o obj/ui.o obj/protocol.o obj/commands.o obj/common.o

obj/server.o: src/server.c include/protocol.h include/server.h include/serverutil.h include/common.h
	mkdir obj
	gcc -Wall -g -c src/server.c -o obj/server.o

obj/serverutil.o: src/serverutil.c include/protocol.h include/server.h
	mkdir obj
	gcc -Wall -g -c src/serverutil.c -o obj/serverutil.o

obj/client.o: src/client.c include/protocol.h include/commands.h include/ui.h
	mkdir obj
	gcc -Wall -g -c src/client.c -o obj/client.o

obj/ui.o: src/ui.c include/ui.h
	mkdir obj
	gcc -Wall -g -c src/ui.c -o obj/ui.o

obj/commands.o: src/commands.c include/commands.h include/common.h
	mkdir obj
	gcc -Wall -g -c src/commands.c -o obj/commands.o

obj/protocol.o: src/protocol.c include/protocol.h
	mkdir obj
	gcc -Wall -g -c src/protocol.c -o obj/protocol.o

obj/common.o: src/common.c include/common.h
	mkdir obj
	gcc -Wall -g -c src/common.c -o obj/common.o


clean:
	rm -rf obj bin