CXX = mpic++
CXXFLAGS = -g3 -Wall -fopenmp -I./CImg
MKDIR = mkdir -p

LIBS = -lm -lmpi -fopenmp -lpthread -lX11

directorios: 
	$(MKDIR) build dist

main.o: directorios main.cpp
	$(CXX) $(CXXFLAGS) -c main.cpp -o build/main.o

all: main.o
	$(CXX) $(CXXFLAGS) -o dist/programa build/main.o $(LIBS)

clean:
	rm -fr *.o a.out programa dist build
	
.DEFAULT_GOAL := all