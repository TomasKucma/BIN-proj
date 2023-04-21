/**
 * Project: BIN project
 * Author: Tomáš Kučma
 * Year: 2022/2023
 * Description: Contains implementation of this project's main
 */

#include "examples.hpp"
#include <iostream>

void test_cgp(CGP cgp, const size_t iteration_count) {
    auto best = cgp.run_evolution(iteration_count);
    auto best_chromosome = std::get<const Chromosome &>(best);
    auto best_fitness = std::get<size_t>(best);
    std::cout << "Best chromosome:\n";             // DEBUG
    cgp.print_chromosome(best_chromosome) << "\n"; // DEBUG
    std::cout << "Best fitness ";                  // DEBUG
    cgp.print_fitness(best_fitness) << "\n\n\n";   // DEBUG
}

int main(int argc, char *argv[]) {
    std::cout << "CGP for 2bit adder:\n\n"; // DEBUG
    test_cgp(ADDER_2b, ADDER_2b_ITERATION_COUNT);
    std::cout << "CGP for 7 input median:\n\n"; // DEBUG
    test_cgp(MEDIAN_7, MEDIAN_7_ITERATION_COUNT);
    std::cout << "CGP for 5 input parity:\n\n"; // DEBUG
    test_cgp(PARITY_5, PARITY_5_ITERATION_COUNT);
    return 0;
}