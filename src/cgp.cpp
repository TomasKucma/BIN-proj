/**
 * Project: BIN project
 * Author: Tomáš Kučma
 * Year: 2022/2023
 * Description: Contains implementation of Caretsian Genetic Programming
 *  algorithm utilizing XOR and Majority function blocks
 */

#include <exception>
#include <iostream>
#include <vector>

using Gene = uint32_t;
using Chromosome = std::vector<Gene>;
using Bitmap = uint64_t;

constexpr size_t BITMAP_SIZE = sizeof(Bitmap) * 8;

// default CGP parameters
constexpr size_t COLS = 5;
constexpr size_t ROWS = 4;
constexpr size_t LBACK = 1;
constexpr size_t LAMBDA = 5;
constexpr size_t IN_COUNT = 7;
constexpr size_t ITERATION_COUNT = 100;
constexpr size_t MUTATION_MAX_COUNT = 5;
const std::vector<std::vector<Bitmap>> EXPECTED_OUTS{
    {0xFFFFFFFFFFFFFFFFU, 0xFFFFFFFFFFFFFFFFU},
    {0x0000000000000000U, 0x0000000000000000U},
    {0x00000000FFFFFFFFU, 0x00000000FFFFFFFFU}};

// ? TODO export function to a new file

constexpr size_t BLOCK_IN_COUNT = 3;
constexpr size_t BLOCK_SIZE = BLOCK_IN_COUNT + 1;

enum Function {
    XOR_00,
    XOR_01,
    XOR_10,
    XOR_11,
    MAJ_000,
    MAJ_001,
    MAJ_010,
    MAJ_011,
    MAJ_100,
    MAJ_101,
    MAJ_110,
    MAJ_111,
    FUNCTION_COUNT,
};

Bitmap simulate_function(const Bitmap &x, const Bitmap &y, const Bitmap &z,
                         const Function &function) {
    switch (function) {
    case XOR_00:
        return ~x ^ ~y;
    case XOR_01:
        return ~x ^ y;
    case XOR_10:
        return x ^ ~y;
    case XOR_11:
        return x ^ y;
    case MAJ_000:
        return (~x & ~y) ^ (~x & ~z) ^ (~y & ~z);
    case MAJ_001:
        return (~x & ~y) ^ (~x & z) ^ (~y & z);
    case MAJ_010:
        return (~x & y) ^ (~x & ~z) ^ (y & ~z);
    case MAJ_011:
        return (~x & y) ^ (~x & z) ^ (y & z);
    case MAJ_100:
        return (x & ~y) ^ (x & ~z) ^ (~y & ~z);
    case MAJ_101:
        return (x & ~y) ^ (x & z) ^ (~y & z);
    case MAJ_110:
        return (x & y) ^ (x & ~z) ^ (y & ~z);
    case MAJ_111:
        return (x & y) ^ (x & z) ^ (y & z);
    default:
        throw std::logic_error(std::string{"Invalid function block "} +
                               std::to_string(function));
    }
}

struct CGP {
    const size_t in_count;
    const size_t out_count;
    const std::vector<std::vector<Bitmap>> &expected_outs;
    const size_t cols;
    const size_t rows;
    const size_t l_back;
    const size_t lambda;
    const size_t mutation_max_count;

    const size_t bit_count;
    const size_t bitmap_count;
    std::vector<std::vector<Bitmap>> ins;

    const size_t chromosome_size;
    std::vector<std::vector<Gene>> column_values;
    std::vector<Chromosome> population;
    std::vector<Bitmap> current_values;

    // Initialization

    void validate_parameters() {
        if (!in_count) {
            throw std::invalid_argument("Input count 0\n");
        }
        if (!out_count) {
            throw std::invalid_argument(
                "Output count 0 (based on expected outputs given)\n");
        }
        if (!cols) {
            throw std::invalid_argument("0 columns\n");
        }
        if (!rows) {
            throw std::invalid_argument("0 rows\n");
        }
        if (!l_back) {
            throw std::invalid_argument("L-back 0\n");
        }
        if (!lambda) {
            throw std::invalid_argument("Lambda 0\n");
        }

        for (const auto &out : expected_outs) {
            if (out.size() != bitmap_count) {
                throw std::invalid_argument(
                    "Expected outputs lengths don't match the amount of all "
                    "possible combinations of the input for given input "
                    "count\n");
            }
        }
    }

    std::vector<std::vector<Bitmap>> generate_input() {
        std::vector<std::vector<Bitmap>> ins(in_count);
        for (size_t i = 0; i < in_count; i++) {
            ins[i].resize(bitmap_count);
            const size_t bit_seq_len = (bit_count >> (i + 1));
            const size_t seq_len = bit_seq_len / BITMAP_SIZE;
            Bitmap input = 0;
            if (seq_len == 0) {
                for (size_t j = 0; j < BITMAP_SIZE / bit_seq_len; j++) {
                    input = ~(~(input << bit_seq_len) << bit_seq_len);
                }
                for (size_t j = 0; j < ins[i].size(); j++) {
                    ins[i][j] = input;
                }
            } else {
                for (size_t j = 0; j < ins[i].size(); j++) {
                    ins[i][j] = ((j / seq_len) % 2 == 0) ? input : ~input;
                }
            }
        }
        return ins;
    }

    std::vector<std::vector<Gene>> generate_column_values() {
        std::vector<std::vector<Gene>> column_values(cols);
        for (size_t col = 0; col < cols; col++) {
            size_t minidx =
                std::max(rows * (col - l_back) + in_count, in_count);
            size_t maxidx = col * rows + in_count;

            column_values[col].resize(in_count + maxidx - minidx);

            auto vals_iter = column_values[col].begin();
            for (size_t k = 0; k < in_count; k++, vals_iter++) {
                *vals_iter = k;
            }
            for (size_t k = minidx; k < maxidx; k++, vals_iter++) {
                *vals_iter = k;
            }
        }
        return column_values;
    }

    CGP(size_t in_count, const std::vector<std::vector<Bitmap>> &expected_outs,
        size_t cols, size_t rows, size_t l_back, size_t lambda,
        size_t mutation_max_count)
        : in_count{in_count}, out_count{expected_outs.size()},
          expected_outs{expected_outs}, cols{cols}, rows{rows}, l_back{l_back},
          lambda{lambda},
          mutation_max_count{mutation_max_count}, bit_count{1UL << in_count},
          bitmap_count{std::max(bit_count / BITMAP_SIZE, 1UL)},
          ins(generate_input()), chromosome_size{cols * rows * BLOCK_SIZE +
                                                 out_count},
          column_values(generate_column_values()),
          population(lambda + 1, Chromosome(chromosome_size)),
          current_values(in_count + cols * rows) {

        validate_parameters();
    };

    // Output

    void print_chromosome(const Chromosome &chromosome) {
        auto chrom_iter = chromosome.cbegin();
        // function blocks
        for (size_t j = 0; j < cols * rows; j++, chrom_iter++) {
            std::cout << "{";
            // inputs
            for (size_t k = 0; k < BLOCK_IN_COUNT; k++, chrom_iter++) {
                std::cout << *chrom_iter << ",";
            }
            // function
            std::cout << *chrom_iter << "}";
        }
        std::cout << " ";
        // output
        for (size_t j = 0; j < out_count; j++, chrom_iter++) {
            std::cout << *chrom_iter << " ";
        }
        std::cout << "\n";
    }

    void print_pop() {
        for (const auto &chromosome : population) {
            print_chromosome(chromosome);
        }
    }

    // Evolution

    size_t get_used_block_count(const Chromosome &chromosome) {
        size_t used_block_count = 0;
        const size_t blocks = (chromosome_size - out_count) / BLOCK_SIZE;
        std::vector<bool> used_blocks(blocks, false);
        // std::cout << "size = " << chromosome_size << ", blocks" << blocks
        //           << "\n"; // DEBUG

        // ? TODO count 3rd xor input?
        size_t i;
        for (i = chromosome_size - 1; i + 1 > 0; i--) {
            if ((i >= chromosome_size - out_count || // if output
                 (i % BLOCK_SIZE < BLOCK_IN_COUNT && // or if input
                  used_blocks[i / BLOCK_SIZE])) &&   // of an active block,
                chromosome[i] >= in_count) { // and connected to another block
                used_block_count += used_blocks[chromosome[i] - in_count]
                                        ? 0
                                        : 1; // count if wasn't active before
                used_blocks[chromosome[i] - in_count] = true;
            }
        }

        std::cout << "used blocks " << used_block_count << "\n"; // DEBUG
        return used_block_count;
    }

    size_t get_fitness(const Chromosome &chromosome) {
        size_t fitness = 0;
        for (size_t i = 0; i < bitmap_count; i++) {
            auto value_iter = current_values.begin();
            // input
            for (const auto &in : ins) {
                *value_iter++ = in[i];
            }
            // funciton blocks
            auto chromosome_iter = chromosome.begin();
            for (size_t j = 0; j < cols * rows; j++, value_iter++) {
                Bitmap x = current_values[*chromosome_iter++];
                Bitmap y = current_values[*chromosome_iter++];
                Bitmap z = current_values[*chromosome_iter++];
                Function function = Function(*chromosome_iter++);
                *value_iter = simulate_function(x, y, z, function);
            }
            // output
            for (const auto &out : expected_outs) {
                Bitmap bit_match =
                    ~(out[i] ^ current_values[*chromosome_iter++]);
                if (bit_count < BITMAP_SIZE) { // mask out ignored bits
                    bit_match &= (1UL << bit_count) - 1;
                }
                for (; bit_match; bit_match = bit_match >> 1) {
                    fitness += (bit_match & 1);
                }
            }
        }
        // if perfect fitness, take block usage into account
        if (fitness == out_count * bit_count) {
            fitness += cols * rows - get_used_block_count(chromosome);
        }
        return fitness;
    }

    void mutate(Chromosome &chromosome) {
        // std::cout << "mutating\n"; // DEBUG
        size_t mutate_count = (rand() % mutation_max_count) + 1;
        for (size_t j = 0; j < mutate_count; j++) {
            size_t i = rand() % chromosome_size;
            size_t col = i / (rows * BLOCK_SIZE);
            // std::cout << "i=" << i << ", col=" << col << "\n"; // DEBUG

            if (i < chromosome_size - out_count) { // block mutation
                chromosome[i] =
                    (i % BLOCK_SIZE) < BLOCK_IN_COUNT
                        ? column_values[col][rand() % column_values[col].size()]
                        : rand() % FUNCTION_COUNT;
                // std::cout << ((i % BLOCK_SIZE) < BLOCK_IN_COUNT
                //                   ? "block"
                //                   : "function")
                //           << " thus " << chromosome[i] << "\n"; // DEBUG
            } else { // output mutation
                chromosome[i] = rand() % (cols * rows + in_count);
            }
        }
    }

    const Chromosome &
    get_best_chromosome(const Chromosome *const parent_ptr = nullptr) {
        auto best_chromosome = population.begin();
        size_t best_fitness = get_fitness(*best_chromosome);
        // std::cout << "default best is "
        //           << (&*best_chromosome == parent_ptr ? "" : "not ")
        //           << "parent\n"; // DEBUG
        for (auto chrom_iter = population.begin() + 1;
             chrom_iter != population.end(); chrom_iter++) {
            size_t fitness = get_fitness(*chrom_iter);
            if (fitness > best_fitness ||
                (fitness == best_fitness &&
                 //  (std::cout << "checking if parent in best chromosome\n",
                 //   true) && // DEBUG
                 &*chrom_iter != parent_ptr)) {
                // std::cout << "not parent\n"; // DEBUG
                best_chromosome = chrom_iter;
                best_fitness = fitness;
                // } else if (fitness == best_fitness) { // DEBUG
                //     std::cout << "parent\n";
            }
        }
        return *best_chromosome;
    }

    void generate_default_pop() {
        for (auto &chromosome : population) {
            auto gene_iter = chromosome.begin();
            // function blocks
            for (size_t j = 0; j < cols * rows; j++, gene_iter++) {
                size_t col = (j / cols);
                // inputs
                for (size_t k = 0; k < BLOCK_IN_COUNT; k++, gene_iter++) {
                    *gene_iter =
                        column_values[col]
                                     [(rand() % (column_values[col].size()))];
                }
                // function
                *gene_iter = rand() % FUNCTION_COUNT;
            }
            // output
            for (size_t j = 0; j < out_count; j++, gene_iter++) {
                *gene_iter = rand() % (in_count + cols * rows);
            }
        }
    }

    void generate_new_pop(const Chromosome &parent) {
        for (auto &chromosome : population) {
            if (&chromosome != &parent) {
                chromosome = parent;
                mutate(chromosome);
                // } else { // DEBUG
                //     std::cout << "skipping parent while generating\n";
            }
        }
    }

    const Chromosome &run_evolution(size_t iter_count) {
        generate_default_pop();
        // print_pop(); // DEBUG

        const Chromosome *parent = nullptr;
        for (size_t i = 0; i < iter_count; i++) {
            parent = &get_best_chromosome(parent);
            std::cout << "Parent fitness " << get_fitness(*parent)
                      << "\n"; // DEBUG
            generate_new_pop(*parent);
            // print_pop(); // DEBUG
        }
        return get_best_chromosome();
    }
};

int main(int argc, char *argv[]) {
    CGP cgp(IN_COUNT, EXPECTED_OUTS, COLS, ROWS, LBACK, LAMBDA,
            MUTATION_MAX_COUNT);
    const Chromosome &best_chromosome = cgp.run_evolution(ITERATION_COUNT);
    cgp.print_chromosome(best_chromosome);
    return 0;
}
