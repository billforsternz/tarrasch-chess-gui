CC:= g++
#-I../thc
CFLAGS := -c -g -std=c++11 -O2 `wx-config --cxxflags`
LIBS:= `wx-config --libs all` -ldl

SRCS:= $(wildcard *.cpp)
OBJS:= $(patsubst %.cpp, %.o, $(SRCS))
TARGET := tarrasch

default: all
all: $(TARGET)

%.o : %.cpp
	$(CC) $(CFLAGS) $< -o $@

$(TARGET) : $(OBJS)
#	$(CC) $(LIBS) $^ -o $(TARGET)

