# BIN project
## Návrh kombinačních obvodů reprezentovaných pomocí XOR-Majority Inverter Graph

  - Author: Tomáš Kučma (xkucma00)
  - Year: 2022/2023
  - Requirements: C++ (C++20), g++

## Project structure

  - `Makefile` - provides targets for building (`build`), running (`run`), cleaning the build and pack output (`clean`), and packing into a zip file (`pack`)
  - `src` - contains source files, specifically:
    - `main.cpp` - contains `main` of the program
    - `cgp.cpp`, `cgp.hpp` - contains implementation of Caretsian Genetic Programming (CGP) algorithm utilizing XOR and Majority function blocks
    - `function.cpp`, `function.hpp` - contains implementation of XOR and Majority blocks
    - `types.hpp` - contains defintion of types used in CGP
    - `examples.hpp` - contains examples of CGP configurations for various circuits
  - `cgp` - project binary, created using `make build` command
  - `build` - temporary folder created when building the binary

## TODO

  - ? reorder bits in inputs/outputs
  - ? clean-up output and remove (or move into macro) debug
  - extend the functionality beyond basic cgp
  - documentation with statistical evaluation
