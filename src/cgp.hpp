#include "function.hpp"
#include "types.hpp"
#include <tuple>
#include <vector>

#ifndef CGP_HPP
#define CGP_HPP

constexpr size_t BLOCK_SIZE = BLOCK_IN_COUNT + 1;

// default CGP parameters
constexpr size_t COLS = 5;
constexpr size_t ROWS = 5;
constexpr size_t L_BACK = 3;
constexpr size_t LAMBDA = 10;
constexpr size_t IN_COUNT = 7;
constexpr size_t ITERATION_COUNT = 1000;
constexpr size_t MUTATION_MAX_COUNT = 10;
const std::vector<std::vector<Bitmap>> EXPECTED_OUTS{
    {0x00000000FFFF0000U, 0xFFFFFFFFFFFFFFFFU},
    {0x0000000000000000U, 0x0000000000000000U},
    {0x00000000FFFFFFFFU, 0x00000000FFFFFFFFU}};

struct CGP {
    // Parameters

    const size_t in_count;
    const size_t out_count;
    const std::vector<std::vector<Bitmap>> &expected_outs;
    const size_t cols;
    const size_t rows;
    const size_t l_back;
    const size_t lambda;
    const size_t mutation_max_count;

    // Internal data

    const size_t bit_count;
    const size_t bitmap_count;
    const size_t max_fitness;
    std::vector<std::vector<Bitmap>> ins;
    const size_t block_count;
    const size_t chromosome_size;
    std::vector<std::vector<Gene>> column_values;
    std::vector<Chromosome> population;
    std::vector<Bitmap> current_values;

    // Initialization

    void validate_parameters();
    std::vector<std::vector<Bitmap>> generate_input();
    std::vector<std::vector<Gene>> generate_column_values();

    CGP(size_t in_count = IN_COUNT,
        const std::vector<std::vector<Bitmap>> &expected_outs = EXPECTED_OUTS,
        size_t cols = COLS, size_t rows = ROWS, size_t l_back = L_BACK,
        size_t lambda = LAMBDA, size_t mutation_max_count = MUTATION_MAX_COUNT)
        : in_count{in_count}, out_count{expected_outs.size()},
          expected_outs{expected_outs}, cols{cols}, rows{rows}, l_back{l_back},
          lambda{lambda},
          mutation_max_count{mutation_max_count}, bit_count{1UL << in_count},
          bitmap_count{std::max(bit_count / BITMAP_SIZE, 1UL)},
          max_fitness{out_count * bit_count},
          ins(generate_input()), block_count{cols * rows},
          chromosome_size{block_count * BLOCK_SIZE + out_count},
          column_values(generate_column_values()),
          population(lambda + 1, Chromosome(chromosome_size)),
          current_values(in_count + block_count) {

        validate_parameters();
    };

    // Output

    void print_parameters();
    void print_chromosome(const Chromosome &chromosome);
    void print_fitness(const size_t &fitness);
    void print_population();

    // Evolution

    size_t get_used_block_count(const Chromosome &chromosome);
    size_t get_fitness(const Chromosome &chromosome);
    void mutate(Chromosome &chromosome);
    std::tuple<size_t, const Chromosome &>
    get_best_chromosome(const Chromosome *const parent_ptr = nullptr);
    void generate_default_population();
    void generate_new_population(const Chromosome &parent);
    std::tuple<size_t, const Chromosome &> run_evolution(size_t iter_count);
};

#endif // CGP_HPP
