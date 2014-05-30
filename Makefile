CXX = g++
CXXFLAGS = -Wall -Wextra -pedantic -std=c++11 -c -ggdb
LDFLAGS = -mconsole

all: test.exe

test.exe: main.o lfwatch_win32.o
	$(CXX) $^ $(LDFLAGS) -o $@

main.o: main.cpp
	$(CXX) $(CXXFLAGS) $^ -o $@

lfwatch_win32.o: lfwatch_win32.cpp lfwatch_win32.h
	$(CXX) $(CXXFLAGS) $< -o $@

.PHONY: clean
clean:
	del main.o lfwatch_win32.o test.exe

