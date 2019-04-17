CC=gcc
AR=ar rcs
CFLAGS=-Wall -pedantic -std=c99 -fPIC
VPATH=src
TARGETS=cutl.so lutl.so


all: $(TARGETS)


%.o: %.c %.h
	$(CC) -c $(CFLAGS) $< -o $@

%.so: %.o
	$(CC) -shared -o $@ $^

lutl.o: CFLAGS+=`pkg-config --cflags lua`

lutl.so: lutl.o cutl.o
	$(CC) -shared -o $@ $^


clean:
	rm -rf *.o *.so


install: $(TARGETS)
	install -Dm644 include/cutl.h "$(DESTDIR)/usr/include/cutl.h"
	install -Dm644 include/lutl.h "$(DESTDIR)/usr/include/lutl.h"
	install -Dm644 example/c_example.c "$(DESTDIR)/usr/share/cutl/example/c_example.c"
	install -Dm644 example/lua_example.c "$(DESTDIR)/usr/share/cutl/example/lua_example.c"
	install -Dm644 example/lua_example.lua "$(DESTDIR)/usr/share/cutl/example/lua_example.lua"
	install -Dm644 example/makefile "$(DESTDIR)/usr/share/cutl/example/makefile"
	install -Dm644 cutl.so "$(DESTDIR)/usr/lib/libcutl.so"
	install -Dm644 lutl.so "$(DESTDIR)/usr/lib/liblutl.so"
	install -Dm644 lutl.so "$(DESTDIR)/usr/lib/lua/5.3/lutl.so"
	install -Dm644 LICENSE "$(DESTDIR)/usr/share/licenses/cutl/LICENSE"


.PHONY=clean install
