#!/usr/bin/make -f

###
# BIN project - Návrh kombinačních obvodů reprezentovaných pomocí XOR-Majority 
#  Inverter Graph
# Author: Tomáš Kučma (xkucma00)
# Year: 2022/2023
###

# Variable definitions

AUTHOR=xkucma00
PROJ_NAME=cgp
PACK_CONTENTS=Makefile src README.md doc
PACK_NAME=BIN-$(AUTHOR).zip
CPP_FLAGS=-std=c++20 -Wall -Werror -O2
SRCS=$(wildcard src/*.cpp)
OBJS=$(SRCS:src/%.cpp=build/%.o)
DEPS=$(OBJS:.o=.d)
STANDARD_ENABLE=-DSTANDARD_VARIANT
OBJS_STANDARD=$(OBJS:.o=_standard.o)
DEPS_STANDARD=$(OBJS:.o=_standard.d)


# Phony targets

.PHONY: all build run build_standard run_standard clean pack

all: build

build: $(PROJ_NAME)

run: $(PROJ_NAME) | logs/
	./$<

build_standard: $(PROJ_NAME)_standard

run_standard: $(PROJ_NAME)_standard | logs_standard/
	./$<

clean:
	rm -rf $(PACK_NAME) $(PROJ_NAME) build

pack: 
	rm -rf $(PACK_NAME)
	zip -r $(PACK_NAME) $(PACK_CONTENTS)


# Build targets

include $(DEPS) $(DEPS_STANDARD)

build/ logs/ logs_standard/:
	mkdir -p $@

build/%.d: src/%.cpp | build/
	g++ -MM -MQ $@ -MQ $(@:.d=.o) -MF $@ $<

build/%.o: src/%.cpp | build/
	g++ $(CPP_FLAGS) -c -o $@ $<

$(PROJ_NAME): $(OBJS)
	g++ $(CPP_FLAGS) -o $@ $^

build/%_standard.d: src/%.cpp | build/
	g++ $(STANDARD_ENABLE) -MM -MQ $@ -MQ $(@:.d=.o) -MF $@ $<

build/%_standard.o: src/%.cpp | build/
	g++ $(CPP_FLAGS) $(STANDARD_ENABLE) -c -o $@ $<

$(PROJ_NAME)_standard: $(OBJS_STANDARD)
	g++ $(CPP_FLAGS) $(STANDARD_ENABLE) -o $@ $^
