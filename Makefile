CXX := g++
CXXFLAGS := -std=c++20 -I/usr/local/include

LDFLAGS := -L/usr/local/lib -lsfml-graphics -lsfml-window -lsfml-system -pthread -ldl -lm

TARGET := CellularAutomata

# Find all .cpp files in current folder and subfolders, excluding obj and cmake build folders
SRC := $(shell find . -name '*.cpp' ! -path './obj/*' ! -path './build/*' ! -path './cmake-build-debug/*')
# Map source files to object files inside obj folder preserving subfolder structure (remove leading ./)
OBJ := $(patsubst ./%,obj/%,$(SRC:.cpp=.o))

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) $^ -o $@ $(LDFLAGS)

obj/%.o: %.cpp
	@mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf obj $(TARGET)

.PHONY: all clean