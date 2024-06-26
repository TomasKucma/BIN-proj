/**
 * Project: BIN project
 * Author: Tomáš Kučma
 * Year: 2022/2023
 * Description: Contains definition of basic types used in this project
 */

#ifndef TYPES_HPP
#define TYPES_HPP

#include <cstdint>
#include <vector>

using Gene = uint32_t;
using Chromosome = std::vector<Gene>;
using Bitmap = uint64_t;

constexpr size_t BITMAP_SIZE = sizeof(Bitmap) * 8;

#endif // TYPES_HPP
