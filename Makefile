# Simple Makefile

# Compiler
CXX = g++

# Source and output
SRC = test.cpp
GLAD = glad/glad.c
OUT = test

# Libraries
LIBS = -lglfw -lGL

# Default target
all:
	$(CXX) -g $(SRC) $(GLAD) -o $(OUT) $(LIBS)

