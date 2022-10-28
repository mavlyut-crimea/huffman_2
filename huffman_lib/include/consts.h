//
// Created by mavlyut on 01/10/22.
//

#ifndef HUFFMAN_CONSTS_H
#define HUFFMAN_CONSTS_H

#include <cstddef>
#include <cstdint>
#include <vector>

using int_t = uint64_t;
using len_t = uint16_t;
using code_t = std::pair<int_t, len_t>;
using char_t = unsigned char;
using weight_t = size_t;
using ind_t = int16_t;
constexpr size_t BYTESIZE = 8;
constexpr size_t ALPHABET_SIZE = (1 << (sizeof(char_t) * BYTESIZE));
constexpr size_t BUFSIZE = 4096;
constexpr size_t BYTEMASK = (1 << BYTESIZE) - 1;

#endif // HUFFMAN_CONSTS_H
