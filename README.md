# BIN project
## Návrh kombinačních obvodů reprezentovaných pomocí XOR-Majority Inverter Graph

 - Author: Tomáš Kučma (xkucma00)
 - Year: 2022/2023
 - Requirements: C++ (c++14)

## Project structure

 - `Makefile` - provides targets for building (`build`), running (`run`), cleaning the build and pack output (`clean`), and packing into a zip file (`pack`)
 - `src` - contains source files, specifically:
   - `cgp.cpp`, `cgp.hpp` - contains implementation of Caretsian Genetic Programming (CGP) algorithm utilizing XOR and Majority function blocks
   - `function.cpp`, `function.hpp` - contains implementation of XOR and Majority blocks
   - `types.cpp` - contains defintion of types used in CGP
 - `cgp` - project binary, created using `make build` command
 - `build` - temporary folder created when building the binary

## TODO
