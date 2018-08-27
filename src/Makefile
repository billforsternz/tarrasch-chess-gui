src := $(wildcard *.cpp)
src := $(filter-out legal-move-generator.cpp, $(src))
src := $(filter-out tabartmsw.cpp, $(src))
obj := $(src:.cpp=.o)

../tarrasch: $(obj)
	$(CXX) -o $@ $^ `wx-config --libs all` -ldl

%.o : %.cpp
	$(CXX) -c -g -std=c++11 `wx-config --cxxflags` $< -o $@
