# BIN project
## Návrh kombinačních obvodů reprezentovaných pomocí XOR-Majority Inverter Graph

 - Author: Tomáš Kučma (xkucma00)
 - Year: 2022/2023
 - Requirements: C++ (C++17)

## Project structure

 - `Makefile` - provides targets for building (`build`), running (`run`), cleaning the build and pack output (`clean`), and packing into a zip file (`pack`)
 - `src` - contains source files, specifically:
   - `main.cpp` - contains `main` of the program
   - `cgp.cpp`, `cgp.hpp` - contains implementation of Caretsian Genetic Programming (CGP) algorithm utilizing XOR and Majority function blocks
   - `function.cpp`, `function.hpp` - contains implementation of XOR and Majority blocks
   - `types.hpp` - contains defintion of types used in CGP
   - `examples.hpp` - Contains examples of CGP configurations for various circuits
 - `cgp` - project binary, created using `make build` command
 - `build` - temporary folder created when building the binary

## TODO

  - ? exclude 3rd input for XOR blocks when counting number of used blocks
  - ? add small look-up for bit-to-fitness conversions
  - ? reorder bits in inputs/outputs
  - clean-up output and remove (or move into macro) debug
  - documentation with statistical evaluation
  - extend the functionality beyond basic cgp
