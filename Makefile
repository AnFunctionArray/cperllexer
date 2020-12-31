CC=gcc-10
CXX=g++-10
CFLAGS=-I$(HOME)/localperl/lib/5.32.0/x86_64-linux/CORE -g -fsanitize=address -O0
CPPFLAGS=$(CFLAGS) -std=gnu++2a
LDFLAGS=-l:libpcre2-8.a -l:llvm-10/lib/libLLVM.so -L$(HOME)/localperl/lib/5.32.0/x86_64-linux/CORE -lperl -lm -lcrypt -lpthread -fsanitize=address
DEPS = main.h ./llvm/llvmgen.h

%.o: %.c ./metaregex/%.c $(DEPS) 
	$(CC) -c -o $@ $< $(CFLAGS)

%.o: ./metaregex/%.cpp $(DEPS) 
	$(CXX) -c -o $@ $< $(CPPFLAGS)

%.o: ./llvmgen/%.cpp $(DEPS)
	$(CXX) -c -o $@ $< $(CPPFLAGS)

%.c: %.xs
	xsubpp $< > $@

cparser: main.o ./metaregex/parseregexfile.o ./metaregex/parseregexfilecpp.o namedcapture.o ./llvm/llvmgen.o perltoc.o
	$(CXX) -o $@ $^ $(LDFLAGS)

all: cparser

clean: 
	rm main.o ./metaregex/parseregexfile.o ./metaregex/parseregexfilecpp.o namedcapture.o ./llvm/llvmgen.o perltoc.o cparser