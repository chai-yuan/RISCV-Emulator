TARGET = Emulator

CXX ?= g++
CXXFLAGS = -std=c++17 -Wall -MMD
LDFLAGS =

SRCS = main.cpp

ARGS ?=

all: $(TARGET)

run: $(TARGET)
	./$(TARGET) $(ARGS)

$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $(TARGET) $(SRCS)

clean:
	rm -f $(TARGET) $(TARGET).d

-include $(TARGET).d

.PHONY: all clean run
