all: id3

tmp/id3.o: src/id3.cpp
	g++ -O2 -g -c --std=c++11 src/id3.cpp -o tmp/id3.o

id3: tmp/id3.o
	g++ -O2 -g --std=c++11 tmp/id3.o -o id3
