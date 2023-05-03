# BIN project
## Návrh kombinačních obvodů reprezentovaných pomocí XOR-Majority Inverter Graph

  - Author: Tomáš Kučma (xkucma00)
  - Year: 2022/2023
  - Requirements: C++ (C++20), g++

## Description

Generates boolean circuits from XOR and 3 Majority function blocks, utilizing Cartesian genetic programming (CGP). CGP is a genetic programming algorithm, which generates solutions in the form of 2D grid of function blocks and evolves them by randomly mutating (changing) their function or their inputs. In this project, this mutation is additionally extended to simplify circuits based on theorem 1 from [a paper on XOR-MAJ circuits](http://msoeken.github.io/papers/2019_aspdac.pdf).

## Project structure

  - `Makefile` - provides targets for building (`build`), running (`run`), cleaning the build and pack output (`clean`), and packing into a zip file (`pack`)
  - `src` - contains source files, specifically:
    - `main.cpp` - contains `main` of the program
    - `cgp.cpp`, `cgp.hpp` - contains implementation of CGP algorithm utilizing XOR and Majority function blocks
    - `function.cpp`, `function.hpp` - contains implementation of XOR and Majority blocks
    - `types.hpp` - contains defintion of types used in CGP
    - `examples.hpp` - contains examples of CGP configurations for various circuits
  - `cgp` - project binary, created using `make build` command
  - `build` - temporary folder used for building

## TODO

  - ? clean-up output and remove (or move into macro) debug
  - statistical evaluation and prep presentation
