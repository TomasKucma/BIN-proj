#!/usr/bin/make -f

###
# BIN project - Návrh kombinačních obvodů reprezentovaných pomocí XOR-Majority 
#  Inverter Graph
# Author: Tomáš Kučma (xkucma00)
# Year: 2022/2023
###

# Variable declarations

AUTHOR=xkucma00
PROJ_NAME=cgp
PACK_CONTENTS=Makefile src README.md doc
PACK_NAME=BIN-$(AUTHOR).zip
CPP_FLAGS=-std=c++17 -Wall -Werror -O2
OBJS=$(patsubst src/%.cpp,build/%.o, $(wildcard src/*.cpp))
DEPS=$(patsubst src/%.cpp,build/%.d, $(wildcard src/*.cpp))


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

include $(DEPS)

build/:
	mkdir -p build

build/%.d: src/%.cpp | build/
	g++ -MM -MQ $@ -MQ $(@:.d=.o) -MF $@ $<

build/%.o: src/%.cpp | build/
	g++ $(CPP_FLAGS) -c -o $@ $<

$(PROJ_NAME): $(OBJS)
	g++ $(CPP_FLAGS) -o $@ $^
