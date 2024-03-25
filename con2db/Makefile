CC=g++

CFLAGS=-std=c++20 -lm -I. -I/usr/include/postgresql -lpq 

DEPS = $(wildcard *.h)
objects := $(patsubst %.cpp, %.o, $(wildcard *.cpp))

all: $(objects)

%.o: %.cpp $(DEPS) 
	$(CC) -c -o $@ $< $(CFLAGS) 

.PHONY: clean

clean:
	rm *.o 
