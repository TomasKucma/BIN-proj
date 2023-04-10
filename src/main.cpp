/**
 * Project: BIN project
 * Author: Tomáš Kučma
 * Year: 2022/2023
 * Description: Contains implementation of this project's main
 */

#include "examples.hpp"
#include <iostream>

void test_cgp(CGP cgp, const size_t iteration_count) {
    auto best = cgp.run_evolution(ADDER_2b_ITERATION_COUNT);
    auto best_chromosome = std::get<const Chromosome &>(best);
    auto best_fitness = std::get<size_t>(best);
    cgp.print_parameters();                // DEBUG
    std::cout << "Best chromosome:\n";     // DEBUG
    cgp.print_chromosome(best_chromosome); // DEBUG
    std::cout << "\nBest fitness ";        // DEBUG
    cgp.print_fitness(best_fitness);       // DEBUG
    std::cout << "\n\n";                   // DEBUG
}

int main(int argc, char *argv[]) {
    std::cout << "CGP for 2bit adder:\n\n"; // DEBUG
    test_cgp(ADDER_2b, ADDER_2b_ITERATION_COUNT);
    std::cout << "CGP for 7 input median:\n\n"; // DEBUG
    test_cgp(MEDIAN_7, MEDIAN_7_ITERATION_COUNT);
    return 0;
}