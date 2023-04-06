#include <cstdint>
#include <vector>

#ifndef TYPES_HPP
#define TYPES_HPP

using Gene = uint32_t;
using Chromosome = std::vector<Gene>;
using Bitmap = uint64_t;

constexpr size_t BITMAP_SIZE = sizeof(Bitmap) * 8;
#endif // TYPES_HPP
