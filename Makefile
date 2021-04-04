all: prepare main

prepare:
	@mkdir -p build

CP_FLAGS=-O3 -pthread -Wall -pedantic -D_GNU_SOURCE -fomit-frame-pointer

main: build/main.o build/time.o build/net_avl.o build/net.o build/http.o build/websocket.o
	cc build/main.o build/time.o build/net_avl.o build/net.o build/http.o build/websocket.o -o main ${CP_FLAGS}

build/time.o: time/time.c time/time.h
	cc -c time/time.c -o build/time.o ${CP_FLAGS}

build/net_avl.o: net/net_avl.c net/net_avl.h net/net_base.h net/http_base.h net/websocket_base.h
	cc -c net/net_avl.c -o build/net_avl.o ${CP_FLAGS}

build/net.o: net/net.c net/net.h build/net_avl.o build/time.o
	cc -c net/net.c -o build/net.o ${CP_FLAGS}

build/http.o: net/http.c net/http.h build/net.o
	cc -c net/http.c -o build/http.o ${CP_FLAGS}

build/websocket.o: net/websocket.c net/websocket.h build/http.o
	cc -c net/websocket.c -o build/websocket.o ${CP_FLAGS}

build/main.o: main.c build/net.o
	cc -c main.c -o build/main.o ${CP_FLAGS}