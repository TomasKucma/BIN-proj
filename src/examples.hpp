/**
 * Project: BIN project
 * Author: Tomáš Kučma
 * Year: 2022/2023
 * Description: Contains examples of CGP configurations for various circuits
 */

#ifndef EXAMPLES_HPP
#define EXAMPLES_HPP

#include "cgp.hpp"

// Adder with 2b inputs

const std::vector<std::vector<Bitmap>> ADDER_2b_EXPECTED_OUTS{
    {0b0101101001011010U}, // out 0
    {0b0011011011001001U}, // out 1
    {0b0000000100110111U}, // out 2
};
const size_t ADDER_2b_ITERATION_COUNT = 2e5;
const CGP ADDER_2b(4, ADDER_2b_EXPECTED_OUTS, 8, 8, 5, 10, 10);

// Median with 7 inputs

const std::vector<std::vector<Bitmap>> MEDIAN_7_EXPECTED_OUTS{
    // out 0
    {0b0000000000000001000000010001011100000001000101110001011101111111,
     0b0000000100010111000101110111111100010111011111110111111111111111},
};
const size_t MEDIAN_7_ITERATION_COUNT = 1e5;
const CGP MEDIAN_7(7, MEDIAN_7_EXPECTED_OUTS, 4, 4, 2, 10, 5);

// Parity with 5 inputs

const std::vector<std::vector<Bitmap>> PARITY_5_EXPECTED_OUTS{
    {0b01101001100101101001011001101001}, // out 0
};
const size_t PARITY_5_ITERATION_COUNT = 2e4;
const CGP PARITY_5(5, PARITY_5_EXPECTED_OUTS, 3, 2, 1, 5, 4);

// Multiplier with 2b inputs

const std::vector<std::vector<Bitmap>> MULT_2b_EXPECTED_OUTS{
    {0b0000010100000101}, // out 0
    {0b0000001101010110}, // out 1
    {0b0000000000110010}, // out 2
    {0b0000000000000001}, // out 3
};
const size_t MULT_2b_ITERATION_COUNT = 4e5;
const CGP MULT_2b(6, MULT_2b_EXPECTED_OUTS, 7, 6, 3, 10, 10);

#endif // EXAMPLES_HPP
