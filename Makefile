CC=gcc
CPP=g++
CFLAGS=
LDFLAGS=-l:libpcre2-8.a
DEPS = main.h

%.o: %.c ./metaregex/%.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)
%.o: ./metaregex/%.cpp
	$(CPP) -c -o $@ $< $(CFLAGS)

cparser: main.o ./metaregex/parseregexfile.o ./metaregex/parseregexfilecpp.o namedcapture.o
	$(CPP) -o $@ $^ $(LDFLAGS)

all: cparser