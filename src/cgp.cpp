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
    out << "Parameters: (" << in_count << "," << out_count << ", " << cols
        << "," << rows << ", " << BLOCK_IN_COUNT << "," << l_back << ", "
        << lambda << "+1," << mutation_max_count << ")";
    return out;
}

std::ostream &CGP::print_chromosome(const Chromosome &chromosome) {
    auto chrom_iter = chromosome.cbegin();
    // function blocks
    for (size_t j = 0; j < block_count; j++, chrom_iter++) {
        out << "([" << j + in_count << "],";
        // inputs
        for (size_t k = 0; k < BLOCK_IN_COUNT; k++, chrom_iter++) {
            out << *chrom_iter << ",";
        }
        // function
        out << *chrom_iter << ")";
    }
    out << "(";
    // output
    for (size_t j = 0; j < out_count; j++, chrom_iter++) {
        out << (j ? "," : "") << *chrom_iter;
    }
    out << ")";
    return out;
}

std::ostream &CGP::print_fitness(const size_t &fitness) {
    if (max_fitness <= fitness) {
        out << max_fitness << "/" << max_fitness << " (block cost "
            << max_fitness + block_count * MAX_BLOCK_COST - fitness << "/"
            << block_count * MAX_BLOCK_COST << ")";
    } else {
        out << fitness << "/" << max_fitness;
    }
    return out;
}

std::ostream &CGP::print_population() {
    for (const auto &chromosome : population) {
        print_chromosome(chromosome) << "\n";
    }
    return out;
}

size_t CGP::get_used_block_cost(const Chromosome &chromosome) {
    size_t used_block_cost = 0;
    std::vector<bool> used_blocks(block_count, false); // ! std::vector<bool>
    // out << "size = " << chromosome_size << ", blocks" << blocks
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

    // out << "used block cost " << used_block_cost << "\n"; // DEBUG
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
    // out << "mutating\n"; // DEBUG
    size_t mutate_count = (rand() % mutation_max_count) + 1;
    for (size_t j = 0; j < mutate_count; j++) {
        size_t i = rand() % chromosome_size;
        size_t col = i / (rows * BLOCK_SIZE);
        // out << "i=" << i << ", col=" << col << "\n"; // DEBUG

        if (i < chromosome_size - out_count) { // block mutation
            chromosome[i] =
                (i % BLOCK_SIZE) < BLOCK_IN_COUNT
                    ? col_values[col][rand() % col_values[col].size()]
                    : rand() % FUNCTION_COUNT;
            // out << ((i % BLOCK_SIZE) < BLOCK_IN_COUNT
            //                   ? "block"
            //                   : "function")
            //           << " thus " << chromosome[i] << "\n"; // DEBUG
            theorem1(chromosome, i);
        } else { // output mutation
            chromosome[i] = rand() % (block_count + in_count);
        }
    }
}

std::tuple<size_t, const Chromosome &>
CGP::get_best_chromosome(const Chromosome *const parent_ptr) {
    auto best_chromosome = population.begin();
    size_t best_fitness = get_fitness(*best_chromosome);
    // out << "default best is "
    //           << (&*best_chromosome == parent_ptr ? "" : "not ")
    //           << "parent\n"; // DEBUG
    for (auto chrom_iter = population.begin() + 1;
         chrom_iter != population.end(); chrom_iter++) {
        size_t fitness = get_fitness(*chrom_iter);
        if (fitness > best_fitness ||
            (fitness == best_fitness &&
             //  (out << "checking if parent in best chromosome\n",
             //   true) && // DEBUG
             &*chrom_iter != parent_ptr)) {
            // out << "not parent\n"; // DEBUG
            best_chromosome = chrom_iter;
            best_fitness = fitness;
            // } else if (fitness == best_fitness) { // DEBUG
            //     out << "parent\n";
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
            //     out << "skipping parent while generating\n";
        }
    }
}

std::tuple<size_t, const Chromosome &> CGP::run_evolution(size_t iter_count) {
    print_parameters() << "\n\n";
    generate_default_population();
    // print_population() << "\n";  // DEBUG

    size_t parent_fitness = 0;
    const Chromosome *parent_ptr = nullptr;
    out << "Generation: chromosome, fitness\n"; // DEBUG
    for (size_t generation = 0; generation < iter_count; generation++) {
        auto [new_fitness, new_parent] = get_best_chromosome(parent_ptr);
        if (new_fitness > parent_fitness) {
            out << generation << ": ";
            print_chromosome(new_parent) << ", ";
            print_fitness(new_fitness) << "\n";
        }
        parent_fitness = new_fitness;
        parent_ptr = &new_parent;
        generate_new_population(*parent_ptr);
        // print_population() << "\n"; // DEBUG
    }
    return get_best_chromosome();
}

#ifndef STANDARD_VARIANT
void CGP::theorem1(Chromosome chromosome, size_t function_index) {
    // out << "theorem1\n";            // DEBUG
    // print_chromosome(chromosome) << "\n"; // DEBUG
    size_t index = function_index - BLOCK_IN_COUNT;

    // check if the function index is actually a function index
    if (index % BLOCK_SIZE != 0) {
        return;
    }

    // check if the chosen block is maj function block
    Function function = static_cast<Function>(chromosome[function_index]);
    if (!is_maj(function)) {
        return;
    }

    // check if its children are xor function blocks
    std::array<size_t, function_in_count(MAJ_111)> in_indexes{};
    for (size_t i = 0; i < in_indexes.size(); index++, i++) {
        if (chromosome[index] < in_count) {
            return;
        }
        in_indexes[i] = (chromosome[index] - in_count) * BLOCK_SIZE;
        function =
            static_cast<Function>(chromosome[in_indexes[i] + BLOCK_IN_COUNT]);
        if (!is_xor(function)) {
            return;
        }
    }

    // check if its children share at least one input
    const size_t invalid_value = in_count + block_count;
    size_t shared_in{invalid_value};
    std::array<size_t, function_in_count(MAJ_111)> remaining_ins{};
    for (size_t i = 0; i < function_in_count(XOR_11); i++) {
        for (size_t j = 0; j < function_in_count(XOR_11); j++) {
            if (chromosome[in_indexes[0] + i] !=
                chromosome[in_indexes[1] + j]) {
                continue;
            }
            for (size_t k = 0; k < function_in_count(XOR_11); k++) {
                if (chromosome[in_indexes[0] + i] !=
                    chromosome[in_indexes[2] + k]) {
                    continue;
                }
                remaining_ins = {chromosome[in_indexes[0] + 1 - i],
                                 chromosome[in_indexes[1] + 1 - j],
                                 chromosome[in_indexes[2] + 1 - k]};
                shared_in = chromosome[in_indexes[0] + i];
            }
        }
    }
    if (shared_in == invalid_value) {
        return;
    }

    // perform replacement using theorem 1
    // first by replacing one of the xor blocks with maj block
    size_t max_index =
        *std::max_element(std::begin(in_indexes), std::end(in_indexes));
    chromosome[max_index + BLOCK_IN_COUNT] = function;
    for (size_t i = 0; i < remaining_ins.size(); i++) {
        chromosome[max_index + i] = remaining_ins[i];
    }
    index = function_index - BLOCK_IN_COUNT;
    // then by replacing the maj block with xor block
    chromosome[function_index] = XOR_11;
    chromosome[index] = shared_in;
    chromosome[index + 1] = max_index / BLOCK_SIZE + in_count;

    // Note: this tranformation may change polarity of the remaining inputs, as
    // it doesn't take into account the original XOR polarities. However it is
    // expected to be called after mutation, when a random function (with random
    // polarity) was chosen, so re-randomizing the polarity has no negative
    // impact

    // out << "theorem1 done\n";                 // DEBUG
    // print_chromosome(chromosome) << "\n";           // DEBUG
}
#endif // STANDARD_VARIANT
