CXXC=g++
CC=gcc
CFLAGS=-g -Wall -pedantic `pcap-config --cflags`
LIBS=`pcap-config --libs`
INCLUDE=src/

test: src/Test.c
	$(CC) $(CFLAGS) $(LIBS) -o bin/$@ $^

