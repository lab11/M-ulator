DEBUG=1

CC=gcc
CPP=g++
CFLAGS=-std=c++0x
LDFLAGS=
DEBUG?=0

TARGET=dnn-compiler
SRC=src
OBJ=obj

CPP_FILES=$(wildcard *.cpp)
CPP_FILES:=$(filter-out non_linear.cpp, $(CPP_FILES))
#HEADER_FILES=$(wildcard *.h)
OBJ_FILES=$(addprefix $(OBJ)/,$(notdir $(CPP_FILES:.cpp=.o)))
#DEP_FILES=$(addprefix $(OBJ)/,$(notdir $(CPP_FILES:.cpp=.d)))

ifeq ($(DEBUG), 1)
	CFLAGS+=-g -DDEBUG
else
	CFLAGS+=-O3
endif

all: dir $(TARGET)

dir:
	@mkdir -p $(OBJ)

non_linear: non_linear.cpp
	$(CPP) $(CFLAGS) -o non_linear non_linear.cpp $(OBJ)/common.o $(OBJ)/fixed.o 
	./non_linear

$(OBJ)/%.o: %.cpp
	$(CPP) $(CFLAGS) -c $< -o $@

$(TARGET): $(OBJ_FILES)
	$(CPP) -o $@ $^ $(LDFLAGS)

clean:
	rm -rf $(TARGET) $(OBJ_FILES) *~

