CXX       := g++-8
CXX_FLAGS := -std=c++17

MKDIR_P := mkdir -p
INCLUDE := include
SRC     := src
BIN     := bin
EXECUTABLE  := OneNorthBank

build: directory $(BIN)/$(EXECUTABLE)

rebuild: clean build

run:
	./$(BIN)/$(EXECUTABLE)

restart: rebuild run

reboot: destroy build run

clean:
	rm $(BIN)/* -f

destroy: clean
	rm accounts.raf -f

$(BIN)/$(EXECUTABLE): $(SRC)/*.cpp
	$(CXX) $(CXX_FLAGS) -I$(INCLUDE) $^ -o $@

directory:
	${MKDIR_P} ${BIN}