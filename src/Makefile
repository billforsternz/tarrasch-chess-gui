CC:= g++
CFLAGS := -c -g -std=c++11 -O2 `wx-config --cxxflags`
LIBS:= `wx-config --libs all` -ldl

SRCS:= $(wildcard *.cpp)
OBJS:= $(patsubst %.cpp, %.o, $(SRCS))
TARGET := ../../tarrasch-chess

default: all
all: $(TARGET)

%.o : %.cpp
	$(CC) $(CFLAGS) $< -o $@

sqlite3.o:
	gcc -c sqlite3.c

$(TARGET) : $(OBJS) sqlite3.o
	$(CC) $(LIBS) $^ -o $(TARGET)
