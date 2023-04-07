/**
 * Project: BIN project
 * Author: Tomáš Kučma
 * Year: 2022/2023
 * Description: Contains examples of CGP configurations for various circuits
 */

#include "cgp.hpp"

const std::vector<std::vector<Bitmap>> ADDER_2b_EXPECTED_OUTS{
    {0b0101101001011010U}, // out 0
    {0b0011011011001001U}, // out 1
    {0b0000000100110111U}, // out 2
};
const size_t ADDER_2b_ITERATION_COUNT = 100000;
const CGP ADDER_2B(4, ADDER_2b_EXPECTED_OUTS, 8, 8, 5, 10, 10);
