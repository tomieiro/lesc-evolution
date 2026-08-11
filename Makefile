CXX ?= g++
CXXFLAGS ?= -std=c++14 -Wall -Wextra -Wpedantic
LDLIBS ?= -lpthread -lfltk -lfltk_images -lX11

TARGET := release/lesc-evolution
SOURCES := src/main/main.cpp src/evolution/base.cpp

.PHONY: all clean run

all: $(TARGET)

$(TARGET): $(SOURCES) headers/base.h headers/gui.h const/macros.h const/matrix.h | release
	$(CXX) $(CXXFLAGS) -o $@ $(SOURCES) $(LDLIBS)

release:
	mkdir -p $@

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)
