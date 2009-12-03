CXXC=g++
CC=gcc
CFLAGS=-g -Wall -pedantic -I/usr/include
LIBS=-L /usr/lib -lpcap
INCLUDE=-Isrc/ -Isrc/util -Isrc/pop -Isrc/imap -Isrc/smtp -Isrc/http

test: src/Test.c
	$(CC) $(CFLAGS) $(LIBS) $(INCLUDE) -o bin/$@ $^

Main: src/Main.cpp util.o Message.o Conversation.o \
    ConversationManager.o PopMessage.o MessageManager.o\
    HttpMessage.o dechunk.o
	$(CXXC) $(CFLAGS) $(LIBS) $(INCLUDE) -o bin/$@ $^

main: src/main.cpp Message.o util.o PopMessage.o 
	$(CXXC) $(CFLAGS) $(LIBS) $(INCLUDE) -o bin/$@ $^

dechunk.o: src/util/dechunk.cpp
	$(CXXC) $(CFLAGS) $(INCLUDE) -c $<

util.o: src/util/util.cpp
	$(CXXC) $(CFLAGS) $(INCLUDE) -c $<

PopMessage.o: src/pop/PopMessage.cpp
	$(CXXC) $(CFLAGS) $(INCLUDE) -c $<

HttpMessage.o: src/http/HttpMessage.cpp
	$(CXXC) $(CFLAGS) $(INCLUDE) -c $<

%.o: src/%.cpp
	$(CXXC) $(CFLAGS) $(INCLUDE) -c $<

clean:
	rm -rf *.o
