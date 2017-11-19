all: id3

a-star: id3.cpp
	g++ -O2 --std=c++11 -o id3 id3.cpp
