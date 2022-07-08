TARGET = lidar
SOURCES = lidartest lidarkit util

INCDIR = include
CXXFLAGS = -I$(INCDIR) -g -std=c++17
LDFLAGS = 

SOURCES_FMT = $(patsubst %, src/%.cpp, $(SOURCES))

all:
	mkdir -p bin/
	$(CXX) $(SOURCES_FMT) $(CXXFLAGS) $(LDFLAGS) -o bin/$(TARGET)

clean:
	rm -r bin