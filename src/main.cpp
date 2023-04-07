/**
 * Project: BIN project
 * Author: Tomáš Kučma
 * Year: 2022/2023
 * Description: Contains implementation of this project's main
 */

#include "examples.hpp"
#include <iostream>

int main(int argc, char *argv[]) {
    CGP cgp(ADDER_2B);
    auto best = cgp.run_evolution(ADDER_2b_ITERATION_COUNT);
    auto best_chromosome = std::get<const Chromosome &>(best);
    auto best_fitness = std::get<size_t>(best);
    cgp.print_parameters();
    std::cout << "Best chromosome:\n";     // DEBUG
    cgp.print_chromosome(best_chromosome); // DEBUG
    std::cout << "\nBest fitness ";        // DEBUG
    cgp.print_fitness(best_fitness);       // DEBUG
    std::cout << "\n\n";                   // DEBUG
    return 0;
}