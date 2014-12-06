
CC=gcc
CXX=g++

CC_FLAGS=
CXX_FLAGS=-std=c++1y

SRC_DIR=src
INC_DIR=include
LIB_DIR=lib
OUT_DIR=out

all: controller client manager

controller client manager:
	$(CXX) -m64 $(CXX_FLAGS) -I$(INC_DIR) -o $(OUT_DIR)/$@ $(SRC_DIR)/$@.cpp -L$(LIB_DIR) -lkvm -lpthread

clean:
	rm -rf $(OUT_DIR)/*