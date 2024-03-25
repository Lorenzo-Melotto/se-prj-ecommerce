SRC_DIR=../commons/src
BIN_DIR=../commons/obj

CC=g++

CFLAGS=-std=c++20 -Wall -I. -lm

HEADS     = $(wildcard $(SRC_DIR)/*.h)
CPPS      = $(wildcard *.cpp)
PRJ_CPPS1 = $(wildcard $(SRC_DIR)/*.cpp)
PRJ_CPPS  = $(filter-out $(SRC_DIR)/progetto_se.cpp, $(PRJ_CPPS1))
OBJS      = $(patsubst %.cpp, %.o, $(CPPS))
OBJS_SRC  = $(patsubst $(SRC_DIR)/%.cpp, $(BIN_DIR)/%.o, $(PRJ_CPPS))

all: test

%.o: %.cpp $(HEADS)
	$(CC) -c -o $@ $< $(CFLAGS)

# build main project files if needed
$(BIN_DIR)/%.o: $(SRC_DIR)/%.cpp $(HEADS)
	$(CC) -c -o $@ $< $(CFLAGS)

test: $(OBJS) $(OBJS_SRC) $(HEADS)
	$(CC) -o $@ $(OBJS) $(OBJS_SRC) $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o $(BIN_DIR)/*.o test