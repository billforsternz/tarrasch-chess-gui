src := $(wildcard *.cpp)
src := $(filter-out legal-move-generator.cpp, $(src))
src := $(filter-out tabartmsw.cpp, $(src))
obj := $(src:.cpp=.o)

../tarrasch: $(obj) sqlite3.o
	$(CXX) -o $@ $^ `wx-config --libs all` -ldl

%.o : %.cpp
	$(CXX) -c -std=c++11 -g `wx-config --cxxflags` $< -o $@

sqlite3.o:
	gcc -c sqlite3.c
