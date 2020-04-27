CXX       := g++-8
CXX_FLAGS := -std=c++17

INCLUDE := include
SRC     := src
BIN     := bin
EXECUTABLE  := OneNorthBank


all: $(BIN)/$(EXECUTABLE)

run: ./$(BIN)/$(EXECUTABLE)

restart: clean all run

fresh: cleaner run

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) $^ -o $@

clean:
	rm $(BIN)/* -f

cleaner: clean
	rm accounts.raf -f