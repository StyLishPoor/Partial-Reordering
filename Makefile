CC      = g++
CPPFLAGS= -Wno-deprecated -O3 -c -m64 -march=native -std=c++17 -DGCC -DRelease
LDFLAGS = -static -O3 -m64
SOURCES = Dynamic_Gorder.cpp Util_my.cpp Graph_my.cpp UnitHeap_my.cpp
OBJECTS = $(SOURCES:.cpp=.o)
EXECUTABLE=Dynamic_Gorder

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE) : $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o : 
	$(CC) $(CPPFLAGS) $< -o $@

clean:
	rm -f *.o
