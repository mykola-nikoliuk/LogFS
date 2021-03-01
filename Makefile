CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=src/main.cpp \
	src/tests/test.cpp src/tests/format.cpp src/tests/file.cpp src/tests/interactive.cpp \
	src/LogFS/LogFS.cpp src/LogFS/Header.cpp src/LogFS/FSIO.cpp src/LogFS/File.cpp src/LogFS/Directory.cpp

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=logfs

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
