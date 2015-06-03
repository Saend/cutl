CC=clang
AR=ar rcs
CFLAGS=-g
LDLIBS=
VPATH=src

all: cutl.a lutl.a lutl.so example


%.o: %.c %.h
	$(CC) -c $(CFLAGS) $< -o $@

%_pic.o: %.c %.h
	$(CC) -c $(CFLAGS) -fPIC $< -o $@

%.a: %.o
	$(AR) $@ $^	


lutl_pic.o: CFLAGS+=`pkg-config --cflags lua`

lutl.a: cutl.o lutl.o

lutl.so: LDFLAGS+=`pkg-config --libs lua`
lutl.so: lutl_pic.o cutl_pic.o
	$(CC) -shared -o $@ $^


example: LDFLAGS+=`pkg-config --libs lua`
example: example.c lutl.o cutl.o


clean:
	rm -rf *.o *.a *.so example
