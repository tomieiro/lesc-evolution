OS := $(shell uname -s)

ifeq ($(OS),Linux)
	FLAGS := -lpthread	-lfltk	-lfltk-images	-w
endif
ifeq ($(OS),Darwin)
		FLAGS := -lpthread	-lfltk	-lfltk-images	-w
endif

ifeq ($(OS),Linux)
	LIBS := sudo apt install mesa-common-dev
endif
ifeq ($(OS),Darwin)
		LIBS := brew install freeglut3
endif

all:	evolution
	g++	-o	release/Main.o	bin/base.o	src/main/main.cpp	$(FLAGS)

evolution:
	g++	-c	src/evolution/base.cpp
	mv	base.o	bin/base.o

clean:
	rm bin/*

install_libs:
	$(LIBS)

run:
	./release/Main.o
