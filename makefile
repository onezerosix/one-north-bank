CXX       := g++-8
CXX_FLAGS := -std=c++17

INCLUDE := include
SRC     := src
BIN     := bin
EXECUTABLE  := OneNorthBank


build: $(BIN)/$(EXECUTABLE)

run: 
	./$(BIN)/$(EXECUTABLE)

restart: clean build run

fresh: cleaner build run

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) $^ -o $@

clean:
	rm $(BIN)/* -f

cleaner: clean
	rm accounts.raf -f