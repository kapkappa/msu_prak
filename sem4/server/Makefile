CC = gcc
CXX = g++
CXXFLAGS = -g -Wall -std=c++17

all: clean cgi prog

prog:
	$(CXX) $(CXXFLAGS) server.cpp -o prog

cgi:
	$(CC) cgi-bin/cgi.c -o cgi; mv cgi cgi-bin/

clean:
	rm -f prog cgi-bin/cgi cgi-bin/*.txt
