CXX       := g++-8
CXX_FLAGS := -std=c++17

INCLUDE := include
SRC     := src
BIN     := bin
EXECUTABLE  := OneNorthBank


all: $(BIN)/$(EXECUTABLE)

run: clean all
	./$(BIN)/$(EXECUTABLE)

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) $^ -o $@

clean:
	rm $(BIN)/*

cleaner:
	rm accounts.raf

fresh: cleaner run