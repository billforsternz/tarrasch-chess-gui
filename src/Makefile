CC=g++
CFLAGS=-g -std=c++11 -O2 `wx-config --cxxflags` -I./t3 -I./thc
LIBS=`wx-config --libs all` -ldl

default: all

all: tarrasch-thc tarrasch-t3

tarrasch-thc:
	$(CC) $(CFLAGS) thc/*.cpp -o tarrasch-thc $(LIBS)
tarrasch-t3:
	$(CC) $(CFLAGS) t3/*.cpp -o tarrasch-t3 $(LIBS)

