#!/usr/bin/make -f

# BIN project - Návrh kombinačních obvodů reprezentovaných pomocí XOR-Majority 
#  Inverter Graph
# Author: Tomáš Kučma (xkucma00)
# Year: 2022/2023

# Variable declarations

AUTHOR=xkucma00
PROJ_NAME=cgp
PACK_CONTENTS=Makefile src README.md doc
PACK_NAME=BIN-$(AUTHOR).zip
CPP_FLAGS=-Wall -Werror -O2


# Phony targets

.PHONY: all build run clean pack

all: build

build: $(PROJ_NAME)

run: $(PROJ_NAME)
	./$(PROJ_NAME)

clean:
	rm -rf $(PACK_NAME) $(PROJ_NAME) build

pack: 
	rm -rf $(PACK_NAME)
	zip -r $(PACK_NAME) $(PACK_CONTENTS)


# Build targets

build/%.o: src/%.cpp
	mkdir -p build
	g++ $(CPP_FLAGS) -c -o $@ $<

build/%.o: src/%.cpp src/%.hpp
	mkdir -p build
	g++ $(CPP_FLAGS) -c -o $@ $<

$(PROJ_NAME): $(patsubst src/%.cpp,build/%.o, $(wildcard src/*.cpp))
	mkdir -p build
	g++ $(CPP_FLAGS) -o $@ $^
