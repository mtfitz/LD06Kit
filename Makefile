TARGET = lidar
SOURCES = lidartest lidarkit util

INCDIR = include
CXXFLAGS = -I$(INCDIR) -O3 -std=c++17
LDFLAGS = 

SOURCES_FMT = $(patsubst %, src/%.cpp, $(SOURCES))

ifeq ($(OS),Windows_NT)
else
	ifeq ($(shell uname), Darwin)
	else
		CXXFLAGS += -pthread
	endif
endif

all:
	mkdir -p bin/
	$(CXX) $(SOURCES_FMT) $(CXXFLAGS) $(LDFLAGS) -o bin/$(TARGET)

clean:
	rm -r bin