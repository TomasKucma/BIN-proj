/**
 * Project: BIN project
 * Author: Tomáš Kučma
 * Year: 2022/2023
 * Description: Contains implementation of Caretsian Genetic Programming
 *  algorithm utilizing XOR and Majority function blocks
 */

#include "cgp.hpp"
#include <exception>
#include <iostream>
#include <vector>

// ? TODO separate function from the rest

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

// Initialization

void CGP::validate_parameters() {
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

std::vector<std::vector<Bitmap>> CGP::generate_input() {
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

std::vector<std::vector<Gene>> CGP::generate_column_values() {
    std::vector<std::vector<Gene>> column_values(cols);
    for (size_t col = 0; col < cols; col++) {
        size_t minidx = std::max(rows * (col - l_back) + in_count, in_count);
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

// Output

void CGP::print_parameters() {
    std::cout << "Parameters: " << in_count << "," << out_count << ", " << cols
              << "," << rows << ", " << lambda << "+1," << mutation_max_count
              << ", " << BLOCK_IN_COUNT << "," << l_back << "\n";
}

void CGP::print_chromosome(const Chromosome &chromosome) {
    auto chrom_iter = chromosome.cbegin();
    // function blocks
    for (size_t j = 0; j < block_count; j++, chrom_iter++) {
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

void CGP::print_fitness(const size_t &fitness) {
    if (max_fitness <= fitness) {
        std::cout << max_fitness << "/" << max_fitness << " (unused blocks "
                  << fitness - max_fitness << "/" << block_count << ")\n";
    } else {
        std::cout << fitness << "/" << max_fitness << "\n";
    }
}

void CGP::print_population() {
    for (const auto &chromosome : population) {
        print_chromosome(chromosome);
    }
}

// Evolution

size_t CGP::get_used_block_count(const Chromosome &chromosome) {
    size_t used_block_count = 0;
    std::vector<bool> used_blocks(block_count, false); // ! std::vector<bool>
    // std::cout << "size = " << chromosome_size << ", blocks" << blocks
    //           << "\n"; // DEBUG

    // ? TODO count 3rd xor input?
    size_t i;
    for (i = chromosome_size - 1; i + 1 > 0; i--) {
        if ((i >= chromosome_size - out_count || // if output
             (i % BLOCK_SIZE < BLOCK_IN_COUNT && // or if input
              used_blocks[i / BLOCK_SIZE])) &&   // of an active block,
            chromosome[i] >= in_count) { // and connected to another block:
            auto block = used_blocks[chromosome[i] - in_count]; // reference
            used_block_count += !block; // count if wasn't active before
            block = true;
        }
    }

    // std::cout << "used blocks " << used_block_count << "\n"; // DEBUG
    return used_block_count;
}

size_t CGP::get_fitness(const Chromosome &chromosome) {
    size_t fitness = 0;
    for (size_t i = 0; i < bitmap_count; i++) {
        auto value_iter = current_values.begin();
        // input
        for (const auto &in : ins) {
            *value_iter++ = in[i];
        }
        // funciton blocks
        auto chromosome_iter = chromosome.begin();
        for (size_t j = 0; j < block_count; j++, value_iter++) {
            Bitmap x = current_values[*chromosome_iter++];
            Bitmap y = current_values[*chromosome_iter++];
            Bitmap z = current_values[*chromosome_iter++];
            Function function = Function(*chromosome_iter++);
            *value_iter = simulate_function(x, y, z, function);
        }
        // output
        for (const auto &out : expected_outs) {
            Bitmap bit_match = ~(out[i] ^ current_values[*chromosome_iter++]);
            if (bit_count < BITMAP_SIZE) { // mask out ignored bits
                bit_match &= (1UL << bit_count) - 1;
            }
            for (; bit_match; bit_match = bit_match >> 1) {
                fitness += (bit_match & 1);
            }
        }
    }
    // if perfect fitness, take block usage into account
    if (fitness == max_fitness) {
        fitness += block_count - get_used_block_count(chromosome);
    }
    return fitness;
}

void CGP::mutate(Chromosome &chromosome) {
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
            chromosome[i] = rand() % (block_count + in_count);
        }
    }
}

const Chromosome &CGP::get_best_chromosome(const Chromosome *const parent_ptr) {
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

void CGP::generate_default_population() {
    for (auto &chromosome : population) {
        auto gene_iter = chromosome.begin();
        // function blocks
        for (size_t j = 0; j < block_count; j++, gene_iter++) {
            size_t col = (j / cols);
            // inputs
            for (size_t k = 0; k < BLOCK_IN_COUNT; k++, gene_iter++) {
                *gene_iter =
                    column_values[col][(rand() % (column_values[col].size()))];
            }
            // function
            *gene_iter = rand() % FUNCTION_COUNT;
        }
        // output
        for (size_t j = 0; j < out_count; j++, gene_iter++) {
            *gene_iter = rand() % (in_count + block_count);
        }
    }
}

void CGP::generate_new_population(const Chromosome &parent) {
    for (auto &chromosome : population) {
        if (&chromosome != &parent) {
            chromosome = parent;
            mutate(chromosome);
            // } else { // DEBUG
            //     std::cout << "skipping parent while generating\n";
        }
    }
}

const Chromosome &CGP::run_evolution(size_t iter_count) {
    std::cout << "Params\n";
    print_parameters();
    std::cout << "\n";
    generate_default_population();
    // print_populationulation(); // DEBUG

    const Chromosome *parent = nullptr;
    for (size_t i = 0; i < iter_count; i++) {
        parent = &get_best_chromosome(parent);
        std::cout << "Parent fitness ";      // DEBUG
        print_fitness(get_fitness(*parent)); // DEBUG
        generate_new_population(*parent);
        // print_populationulation(); // DEBUG
    }
    return get_best_chromosome();
}

int main(int argc, char *argv[]) {
    CGP cgp(IN_COUNT, EXPECTED_OUTS, COLS, ROWS, LBACK, LAMBDA,
            MUTATION_MAX_COUNT);
    const Chromosome &best_chromosome = cgp.run_evolution(ITERATION_COUNT);
    cgp.print_chromosome(best_chromosome);
    std::cout << "Best fitness ";                        // DEBUG
    cgp.print_fitness(cgp.get_fitness(best_chromosome)); // DEBUG
    return 0;
}
