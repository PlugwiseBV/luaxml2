CC=gcc
CFLAGS=-Wall -fPIC -I/usr/include/lua5.1 -I/usr/include/libxml2 -shared
LDFLAGS=-lxml2 -llua5.1

all: 
	$(CC) $(CFLAGS) -o luaxml2.so src/luaxml2.c $(LDFLAGS) 

install:
	cp luaxml2.so /usr/local/lib/lua/5.1/

clean:
	rm -f luaxml2.so
