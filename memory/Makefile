CC      = g++
CPPFLAGS= -Wno-deprecated -O3 -c -g -m64 -march=native -std=c++17 -DGCC -DRelease
LDFLAGS = -static -O3 -m64
SOURCES = DBG.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE = DBG

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE) : $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o : 
	$(CC) $(CPPFLAGS) $< -o $@

clean:
	rm -f *.o

