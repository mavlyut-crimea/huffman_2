//
// Created by mavlyut on 01/10/22.
//

#include "include/counter.h"
#include <array>
#include <fstream>

counter::counter() {
  cnts.fill(0);
}

counter::~counter() = default;

void counter::append(char x) {
  cnts[static_cast<char_t>(static_cast<int>(x) + ALPHABET_SIZE / 2)]++;
}

void counter::read_from_file(char const* in) {
  std::ifstream fin(in, std::ifstream::in);
  auto rdbuf = fin.rdbuf();
  std::array<char, BUFSIZE> buf{};
  std::streamsize sz = -1;
  while (sz != 0) {
    sz = rdbuf->sgetn(buf.data(), BUFSIZE);
    for (size_t i = 0; i < sz; i++)
      append(buf[i]);
  }
  fin.close();
}

weight_t counter::operator[](ind_t x) const {
  return cnts[x];
}
