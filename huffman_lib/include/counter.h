//
// Created by mavlyut on 01/10/22.
//

#ifndef HUFFMAN_COUNTER_H
#define HUFFMAN_COUNTER_H

#include "consts.h"
#include <array>
#include <string>

struct counter {
  counter();
  ~counter();

  void append(char);
  void read_from_file(char const*);
  weight_t operator[](ind_t) const;

private:
  std::array<weight_t, ALPHABET_SIZE> cnts{};
};

#endif // HUFFMAN_COUNTER_H
