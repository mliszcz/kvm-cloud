
CC=gcc
CXX=g++

CC_FLAGS=
CXX_FLAGS=-std=c++1y

SRC_DIR=src
INC_DIR=include
LIB_DIR=lib
OUT_DIR=out

all: controller

controller:
	$(CXX) -m64 $(CXX_FLAGS) -I$(INC_DIR) -o $(OUT_DIR)/controller $(SRC_DIR)/controller.cpp -L$(LIB_DIR) -l:$(LIB_DIR)/libkvm.a 

clean:
	rm -rf $(OUT_DIR)/*