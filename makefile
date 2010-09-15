CC=g++
CFLAGS=-c -Wall

all: part
	
part: main.o kl.o
	$(CC) main.o kl.o -o part

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

kl.o: kl.cpp
	$(CC) $(CFLAGS) kl.cpp

clean:
	rm -rf *.o part
