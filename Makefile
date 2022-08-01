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

ifeq ($(PREFIX),)
    PREFIX := /usr/local
endif

ifeq ($(OS),Windows_NT)
	$(error Not compatible with Windows!)
else
	ifeq ($(shell uname), Darwin)
	else
		CXXFLAGS += -pthread
	endif
endif

all: lib/$(LIB_TARGET).a
	@mkdir -p bin build lib

	@echo "Linking static library..."
	@$(CXX) $(LIB_TARGETS_FMT) $(CXXFLAGS) $(LIB_LDFLAGS) -shared -o lib/lib$(LIB_TARGET).a

	@echo "(UNIMPLEMENTED) Linking dynamic library..."
	#@$(CXX) $(LIB_TARGETS_FMT) $(CXXFLAGS) $(LIB_LDFLAGS) -dynamiclib -o lib/lib$(LIB_TARGET).dylib

	@echo "Compiling executable..."
	@$(CXX) $(EXE_SOURCES_FMT) $(CXXFLAGS) $(EXE_LDFLAGS) -o bin/$(EXE_TARGET)

lib/$(LIB_TARGET).a: $(LIB_TARGETS_FMT)

build/%.o: src/%.cpp
	@mkdir -p bin build lib

	@echo "Compiling $@..."
	@$(CXX) $< $(CXXFLAGS) $(LIB_LDFLAGS) -c -fpic -o $@

.PHONY: clean install

install:
	@echo "Installing headers to $(PREFIX)/include/LD06Kit..."
	@cp -r include/LD06Kit $(PREFIX)/include/.
	@echo "Installing libraries to $(PREFIX)/lib..."
	@cp -r lib/* $(PREFIX)/lib/.

clean:
	@echo "Cleaning..."
	@rm -rf bin build lib ld06kit.egg-info