CXXC=g++
CC=gcc
CFLAGS=-g -Wall -pedantic `pcap-config --cflags`
LIBS=`pcap-config --libs`
INCLUDE=-Isrc/{,util,pop,imap,smtp,http}

test: src/Test.c
	$(CC) $(CFLAGS) $(LIBS) $(INCLUDE) -o bin/$@ $^

Main: src/Main.cpp util.o Message.o Conversation.o \
    ConversationManager.o PopMessage.o MessageManager.o
	$(CXXC) $(CFLAGS) $(LIBS) $(INCLUDE) -o bin/$@ $^

main: src/main.cpp Message.o util.o PopMessage.o
	$(CXXC) $(CFLAGS) $(LIBS) $(INCLUDE) -o bin/$@ $^

util.o: src/util/util.cpp
	$(CXXC) $(CFLAGS) $(INCLUDE) -c $<

PopMessage.o: src/pop/PopMessage.cpp
	$(CXXC) $(CFLAGS) $(INCLUDE) -c $<

%.o: src/%.cpp
	$(CXXC) $(CFLAGS) $(INCLUDE) -c $<

clean:
	rm -rf *.o
