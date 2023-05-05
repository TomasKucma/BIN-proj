/**
 * Project: BIN project
 * Author: Tomáš Kučma
 * Year: 2022/2023
 * Description: Contains implementation of the project's main
 */

#include "examples.hpp"
#include <fstream>
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

void test_cgp_configurations(const CGP &cgp, const size_t iteration_count,
                             std::string file_prefix) {
    constexpr size_t experiment_count = 10;
    for (size_t pop_size = 5; pop_size <= 20; pop_size += 5) {
        for (size_t i = 0; i < experiment_count; i++) {
            std::cout << "wtf\n";
            std::cout << (cgp.lambda + 1) * iteration_count / pop_size << "\n";
            std::ofstream out(std::string{"logs/"} + file_prefix + "_" +
                              std::to_string(pop_size) + "_" +
                              std::to_string(i) + ".log");
            CGP config_cgp(cgp.in_count, cgp.expected_outs, cgp.cols, cgp.rows,
                           cgp.l_back, pop_size - 1, cgp.mutation_max_count,
                           out);
            config_cgp.run_evolution((cgp.lambda + 1) * iteration_count /
                                     pop_size);
        }
    }
}

int main(int argc, char *argv[]) {
    // std::cout << "CGP for 2bit adder:\n\n"; // DEBUG
    // test_cgp(ADDER_2b, ADDER_2b_ITERATION_COUNT);
    test_cgp_configurations(ADDER_2b, ADDER_2b_ITERATION_COUNT, "adder2b");
    // std::cout << "CGP for 7 input median:\n\n"; // DEBUG
    // test_cgp(MEDIAN_7, MEDIAN_7_ITERATION_COUNT);
    test_cgp_configurations(MEDIAN_7, MEDIAN_7_ITERATION_COUNT, "median7");
    // std::cout << "CGP for 5 input parity:\n\n"; // DEBUG
    // test_cgp(PARITY_5, PARITY_5_ITERATION_COUNT);
    test_cgp_configurations(PARITY_5, PARITY_5_ITERATION_COUNT, "parity5");
    // std::cout << "CGP for 2bit input multiplier:\n\n"; // DEBUG
    // test_cgp(MULT_2b, MULT_2b_ITERATION_COUNT);
    test_cgp_configurations(MULT_2b, MULT_2b_ITERATION_COUNT, "mult2b");
    return 0;
}
