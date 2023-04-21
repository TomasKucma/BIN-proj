/**
 * Project: BIN project
 * Author: Tomáš Kučma
 * Year: 2022/2023
 * Description: Contains implementation of Caretsian Genetic Programming
 *  algorithm utilizing XOR and Majority function blocks
 */

#include "cgp.hpp"
#include "function.hpp"
#include <bit>
#include <iostream>
#include <stdexcept>
#include <tuple>
#include <vector>

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
    std::vector<std::vector<Bitmap>> ins(in_count,
                                         std::vector<Bitmap>(bitmap_count));
    for (size_t i = 0; i < in_count; i++) {
        const size_t bit_seq_len = (bit_count >> (i + 1));
        const size_t seq_len = bit_seq_len / BITMAP_SIZE;
        Bitmap input = 0;
        if (seq_len > 0) {
            for (size_t j = 0; j < ins[i].size(); j++) {
                ins[i][j] = ((j / seq_len) % 2 == 0) ? input : ~input;
            }
        } else {
            for (size_t j = 0; j < BITMAP_SIZE / bit_seq_len; j++) {
                input = ~(~(input << bit_seq_len) << bit_seq_len);
            }
            for (size_t j = 0; j < ins[i].size(); j++) {
                ins[i][j] = input;
            }
        }
    }
    return ins;
}

std::vector<std::vector<Gene>> CGP::generate_col_values() {
    std::vector<std::vector<Gene>> col_values(cols);
    for (size_t col = 0; col < cols; col++) {
        size_t minidx = std::max(rows * (col - l_back) + in_count, in_count);
        size_t maxidx = col * rows + in_count;

        col_values[col].resize(in_count + maxidx - minidx);

        auto vals_iter = col_values[col].begin();
        for (size_t k = 0; k < in_count; k++, vals_iter++) {
            *vals_iter = k;
        }
        for (size_t k = minidx; k < maxidx; k++, vals_iter++) {
            *vals_iter = k;
        }
    }
    return col_values;
}

std::ostream &CGP::print_parameters() {
    std::cout << "Parameters: (" << in_count << "," << out_count << ", " << cols
              << "," << rows << ", " << BLOCK_IN_COUNT << "," << l_back << ", "
              << lambda << "+1," << mutation_max_count << ")";
    return std::cout;
}

std::ostream &CGP::print_chromosome(const Chromosome &chromosome) {
    auto chrom_iter = chromosome.cbegin();
    // function blocks
    for (size_t j = 0; j < block_count; j++, chrom_iter++) {
        std::cout << "([" << j + in_count << "],";
        // inputs
        for (size_t k = 0; k < BLOCK_IN_COUNT; k++, chrom_iter++) {
            std::cout << *chrom_iter << ",";
        }
        // function
        std::cout << *chrom_iter << ")";
    }
    std::cout << "(";
    // output
    for (size_t j = 0; j < out_count; j++, chrom_iter++) {
        std::cout << (j ? "," : "") << *chrom_iter;
    }
    std::cout << ")";
    return std::cout;
}

std::ostream &CGP::print_fitness(const size_t &fitness) {
    if (max_fitness <= fitness) {
        std::cout << max_fitness << "/" << max_fitness << " (block cost "
                  << max_fitness + block_count * MAX_BLOCK_COST - fitness << "/"
                  << block_count * MAX_BLOCK_COST << ")";
    } else {
        std::cout << fitness << "/" << max_fitness;
    }
    return std::cout;
}

std::ostream &CGP::print_population() {
    for (const auto &chromosome : population) {
        print_chromosome(chromosome) << "\n";
    }
    return std::cout;
}

size_t CGP::get_used_block_cost(const Chromosome &chromosome) {
    size_t used_block_cost = 0;
    std::vector<bool> used_blocks(block_count, false); // ! std::vector<bool>
    // std::cout << "size = " << chromosome_size << ", blocks" << blocks
    //           << "\n"; // DEBUG

    size_t i;
    for (i = chromosome_size - 1; i + 1 > 0; i--) {
        if ((i >= chromosome_size - out_count || // if output,
             (used_blocks[i / BLOCK_SIZE] &&     // or if active block
              i % BLOCK_SIZE <                   // and is input,
                  function_in_count(static_cast<Function>(
                      chromosome[i - i % BLOCK_SIZE + BLOCK_IN_COUNT])))) &&
            chromosome[i] >= in_count) { // then if connected to another block:
            auto block = used_blocks[chromosome[i] - in_count]; // reference
            used_block_cost +=
                !block *
                function_cost(static_cast<Function>(
                    chromosome[(chromosome[i] - in_count) * BLOCK_SIZE +
                               BLOCK_IN_COUNT]));
            block = true;
        }
    }

    // std::cout << "used block cost " << used_block_cost << "\n"; // DEBUG
    return used_block_cost;
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
            fitness += std::popcount(bit_match);
        }
    }
    // if perfect fitness, take block usage into account
    if (fitness == max_fitness) {
        fitness +=
            block_count * MAX_BLOCK_COST - get_used_block_cost(chromosome);
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
                    ? col_values[col][rand() % col_values[col].size()]
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

std::tuple<size_t, const Chromosome &>
CGP::get_best_chromosome(const Chromosome *const parent_ptr) {
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
    return std::tie(best_fitness, *best_chromosome);
}

void CGP::generate_default_population() {
    for (auto &chromosome : population) {
        auto gene_iter = chromosome.begin();
        // function blocks
        for (size_t j = 0; j < block_count; j++, gene_iter++) {
            size_t col = (j / rows);
            // inputs
            for (size_t k = 0; k < BLOCK_IN_COUNT; k++, gene_iter++) {
                *gene_iter =
                    col_values[col][(rand() % (col_values[col].size()))];
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

std::tuple<size_t, const Chromosome &> CGP::run_evolution(size_t iter_count) {
    print_parameters() << "\n\n";
    generate_default_population();
    // print_population() << "\n";  // DEBUG

    size_t parent_fitness = 0;
    const Chromosome *parent_ptr = nullptr;
    for (size_t generation = 0; generation < iter_count; generation++) {
        auto [new_fitness, new_parent] = get_best_chromosome(parent_ptr);
        if (new_fitness > parent_fitness) {
            std::cout << "Generation " << generation << "\n";
            std::cout << "Parent chromosome:\n";  // DEBUG
            print_chromosome(new_parent) << "\n"; // DEBUG
            std::cout << "Parent fitness ";       // DEBUG
            print_fitness(new_fitness) << "\n\n"; // DEBUG
        }
        parent_fitness = new_fitness;
        parent_ptr = &new_parent;
        generate_new_population(*parent_ptr);
        // print_population() << "\n"; // DEBUG
    }
    return get_best_chromosome();
}
