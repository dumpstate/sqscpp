BIN = ./bin
SRC = ./src

dev:
	docker compose run --rm app

clean:
	rm -rf $(BIN)

$(BIN):
	mkdir -p bin

$(BIN)/main: $(SRC)/*.cpp
	g++ -std=c++23 $(SRC)/main.cpp -o $(BIN)/main

build: $(BIN) $(BIN)/main
