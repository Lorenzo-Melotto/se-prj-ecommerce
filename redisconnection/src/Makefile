INC_DIR=.
OBJ_DIR=../obj
BIN_DIR=../bin

CC=g++

CFLAGS=-std=c++20 -g -fpermissive -L/usr/local/lib -Wall -I. -I$(INC_DIR) -lm -lhiredis

DEPS=$(wildcard $(INC_DIR)/*.h)
SRC=$(wildcard *.cpp)
OBJ=$(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRC))

all: $(OBJ_DIR)/$(OBJ)

$(OBJ_DIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(OBJ_DIR)/*.o