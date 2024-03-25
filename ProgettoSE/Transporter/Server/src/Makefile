INC_DIR=../../../../redisconnection/src
OBJ_DIR=../obj
BIN_DIR=../bin
COM_DIR_SRC=../../../commons/src
COM_DIR_OBJ=../../../commons/obj

CC=g++

CFLAGS=-std=c++20 -g -ggdb -fpermissive -L/usr/local/lib -Wall -I. -I$(INC_DIR) -I$(COM_DIR_SRC) -I../../../../con2db -I/usr/include/postgresql -lpq -lm -lhiredis

DEPS=$(wildcard $(INC_DIR)/*.h) #creates a list containing all the header files that this project depends on
SRC=$(wildcard *.cpp) #creates a list containing all the .cpp files in the current directory
OBJ=$(patsubst %.cpp, $(OBJ_DIR)/%.o, $(SRC)) #replaces all the name.cpp with ../obj/name.o in the list SRC
COM_SRC=$(wildcard $(COM_DIR_SRC)/*.cpp)
COM_OBJ=$(patsubst $(COM_DIR_SRC)/%.cpp, $(COM_DIR_OBJ)/%.o, $(COM_SRC)) #replaces all the name.cpp with ../obj/name.o in the list SRC

redisconnection_OBJ=$(wildcard ../../../../redisconnection/obj/*.o) #creates a list containing all the redis object files

postgresql_OBJ=$(wildcard ../../../../con2db/*.o) #creates a list containing all the postgresql object files

all: redisconnection con2db $(BIN_DIR)/TransporterServer

con2db:
	$(MAKE) -C ../../../../con2db

redisconnection:
	$(MAKE) -C ../../../../redisconnection/src

$(OBJ_DIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(COM_DIR_OBJ)/%.o: $(COM_DIR_SRC)/%.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

$(BIN_DIR)/TransporterServer: $(OBJ) $(COM_OBJ) $(DEPS)
	$(CC) -o $@ $(OBJ) $(COM_OBJ) $(redisconnection_OBJ) $(postgresql_OBJ) $(CFLAGS)

.PHONY: clean

clean:
	rm -f $(OBJ_DIR)/*.o $(BIN_DIR)/* $(COM_DIR_OBJ)/*

