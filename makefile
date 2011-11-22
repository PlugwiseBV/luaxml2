
all: 
	gcc -Wall -fPIC -I/usr/include/lua5.1 -I/usr/include/libxml2 -lxml2 -O2 -shared -o luaxml2.so src/luaxml2.c

clean:
	rm -f luaxml2.so


