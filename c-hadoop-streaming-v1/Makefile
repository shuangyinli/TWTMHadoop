export CC = gcc
export CXX = g++
export CFLAGS = -Wall -g -O3

INSTALL_PATH=bin/
BIN = mapper reducer cal_para init_model test-mapper
OBJ = twtm-estimate.o  twtm-inference.o model.o twtm-learn.o
.PHONY: clean all

all: $(BIN)

test-mapper:test-mapper.cpp twtm-inference.o model.o twtm-estimate.o twtm-learn.o
init_model:init_model.cpp
cal_para:cal_para.cpp model.o twtm-learn.o
reducer:reducer.cpp twtm-learn.o model.o
mapper:mapper.cpp twtm-inference.o twtm-estimate.o model.o
twtm-estimate.o:twtm-estimate.c utils.h model.h twtm-estimate.h
twtm-inference.o: twtm-inference.c utils.h twtm-inference.h model.h
twtm-learn.o: twtm-learn.c utils.h twtm-learn.h model.h
model.o: model.cpp model.h 

$(BIN) : 
	$(CXX) $(CFLAGS) -o $@ $(filter %.cpp %.o %.c, $^)

$(OBJ) :
	$(CXX) -c $(CFLAGS) -o $@ $(filter %.cpp %.c, $^)

install:
	cp -f -r $(BIN) $(INSTALL_PATH)

clean:
	$(RM) $(OBJ) $(BIN) *~
