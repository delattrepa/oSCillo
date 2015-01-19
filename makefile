CC=g++
CFLAGS=-c -Wall -pthread
LDFLAGS= -L/usr/local/lib/ -lserial -lSDL2main -lSDL2
SOURCES=m4.cpp 
OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=sdl

all: $(SOURCES) $(EXECUTABLE)
	
$(EXECUTABLE): $(OBJECTS) 
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
 
