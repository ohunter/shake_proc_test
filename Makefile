CXX=g++
CXXFLAGS=-g -Wall -std=c++17 -O3
LIB=-lcrypto
MP=-fopenmp

SRC=src/

all: clean seq par

seq:
	$(CXX) $(CXXFLAGS) $(SRC)main.cpp -o seq $(LIB)

par:
	$(CXX) $(CXXFLAGS) $(SRC)main.cpp -o par $(LIB) $(MP)

clean:
	rm -rf seq par