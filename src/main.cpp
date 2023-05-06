/**
 * Project: BIN project
 * Author: Tomáš Kučma
 * Year: 2022/2023
 * Description: Contains implementation of the project's main
 */

#include "examples.hpp"
#include <fstream>
#include <iostream>

#ifdef STANDARD_VARIANT
constexpr const char *out_folder = "logs_standard/";
#else  // STANDARD_VARIANT
constexpr const char *out_folder = "logs/";
#endif // STANDARD_VARIANT

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
            std::ofstream out(std::string{out_folder} + "/" + file_prefix +
                              "_" + std::to_string(pop_size) + "_" +
                              std::to_string(i) + ".log");
            CGP config_cgp(cgp.in_count, cgp.expected_outs, cgp.cols, cgp.rows,
                           cgp.l_back, pop_size - 1, cgp.mutation_max_count,
                           out);
            config_cgp.run_evolution((cgp.lambda + 1) * iteration_count /
                                     pop_size);
        }
    }
}

void run_statistics() {
    std::cout << "Generating statistics\n\n";
    std::cout << "2bit adder\n";
    test_cgp_configurations(ADDER_2b, ADDER_2b_ITERATION_COUNT, "adder2b");
    std::cout << "7 input median\n";
    test_cgp_configurations(MEDIAN_7, MEDIAN_7_ITERATION_COUNT, "median7");
    std::cout << "5 input parity\n";
    test_cgp_configurations(PARITY_5, PARITY_5_ITERATION_COUNT, "parity5");
    std::cout << "2bit input multiplier\n";
    test_cgp_configurations(MULT_2b, MULT_2b_ITERATION_COUNT, "mult2b");
}

void run_examples() {
    std::cout << "Running with examples\n\n";
    std::cout << "CGP for 2bit adder:\n\n";
    test_cgp(ADDER_2b, ADDER_2b_ITERATION_COUNT);
    std::cout << "CGP for 7 input median:\n\n";
    test_cgp(MEDIAN_7, MEDIAN_7_ITERATION_COUNT);
    std::cout << "CGP for 5 input parity:\n\n";
    test_cgp(PARITY_5, PARITY_5_ITERATION_COUNT);
    std::cout << "CGP for 2bit input multiplier:\n\n";
    test_cgp(MULT_2b, MULT_2b_ITERATION_COUNT);
}

int main(int argc, char *argv[]) {
    srand(time(NULL));
    run_examples();
    std::cout << std::string(80, '-') << "\n";
    run_statistics();
    return 0;
}
