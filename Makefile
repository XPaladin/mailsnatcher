CXXC=g++
CC=gcc
CFLAGS=-g -Wall -pedantic `pcap-config --cflags`
LIBS=`pcap-config --libs`
INCLUDE=-Isrc/{,util,pop,imap,smtp,http}

test: src/Test.c
	$(CC) $(CFLAGS) $(LIBS) -o bin/$@ $^

main: src/main.cpp Message.o util.o
	$(CXXC) $(CFLAGS) $(LIBS) $(INCLUDE) -o bin/$@ $^

util.o: src/util/util.cpp
	$(CXXC) $(CFLAGS) -c $<

%.o: src/%.cpp
	$(CXXC) $(CFLAGS) -c $<

clean:
	rm -rf *.o
