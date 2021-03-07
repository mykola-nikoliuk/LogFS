CC=g++
CFLAGS=-c -Wall
LDFLAGS=
SOURCES=tests/main.cpp \
	tests/test.cpp tests/file.cpp tests/format.cpp \
	tests/reinit.cpp tests/interactive.cpp \
	src/LogFS.cpp src/LogFSHeader.cpp src/LogFSFile.cpp \
	src/LogFSDirectory.cpp src/LogFSStorageIO.cpp

OBJECTS=$(SOURCES:.cpp=.o)
EXECUTABLE=testFS

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(LDFLAGS) $(OBJECTS) -o $@

.cpp.o:
	$(CC) $(CFLAGS) $< -o $@
