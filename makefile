
all: 
	gcc -Wall -fPIC -I/usr/include/lua5.1 -I/usr/include/libxml2 -shared -o luaxml2.so src/luaxml2.c -lxml2 -llua5.1

install:
	cp luaxml2.so /usr/local/lib/lua/5.1/

clean:
	rm -f luaxml2.so


