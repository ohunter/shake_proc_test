CXX=g++
CXXFLAGS=-g -Wall
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