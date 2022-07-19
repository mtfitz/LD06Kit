EXE_TARGET = lidar
LIB_TARGET = ld06kit
EXE_SOURCES = lidartest
LIB_SOURCES = lidarkit util
SOURCES = EXE_SOURCES LIB_SOURCES

CXXFLAGS = -Iinclude -O3 -std=c++17 -Wall -Werror
EXE_LDFLAGS = -Llib -lld06kit
LIB_LDFLAGS = 

EXE_SOURCES_FMT = $(patsubst %, src/%.cpp, $(EXE_SOURCES))
LIB_SOURCES_FMT = $(patsubst %, src/%.cpp, $(LIB_SOURCES))
LIB_TARGETS_FMT = $(patsubst %, build/%.o, $(LIB_SOURCES))

ifeq ($(OS),Windows_NT)
	$(error Not compatible with Windows!)
else
	ifeq ($(shell uname), Darwin)
	else
		CXXFLAGS += -pthread
	endif
endif

all: lib/$(LIB_TARGET).so
	@mkdir -p bin build lib

	@echo "Linking library..."
	@$(CXX) $(LIB_TARGETS_FMT) $(CXXFLAGS) $(LIB_LDFLAGS) -shared -o lib/lib$(LIB_TARGET).so

	@echo "Compiling executable..."
	@$(CXX) $(EXE_SOURCES_FMT) $(CXXFLAGS) $(EXE_LDFLAGS) -o bin/$(EXE_TARGET)

lib/$(LIB_TARGET).so: $(LIB_TARGETS_FMT)

build/%.o: src/%.cpp
	@mkdir -p bin build lib

	@echo "Compiling $@..."
	@$(CXX) $< $(CXXFLAGS) $(LIB_LDFLAGS) -c -fpic -o $@

.PHONY: clean

clean:
	@echo "Cleaning..."
	@rm -rf bin build lib