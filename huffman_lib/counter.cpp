//
// Created by mavlyut on 01/10/22.
//

#include "include/counter.h"
#include <array>
#include <fstream>

counter::counter() : used(0) {
  cnts.fill(0);
}

counter::~counter() = default;

void counter::append(char x) {
  if (cnts[static_cast<char_t>(static_cast<int>(x) + ALPHABET_SIZE / 2)]++ == 0)
    used++;
}

void counter::normalize() {
  if (used < 2) {
    cnts[0]++;
    if (used < 1)
      cnts[1]++;
  }
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
  normalize();
}

weight_t counter::get_truth_cnt(ind_t i) const {
  return cnts[i] - (i == 0 && used < 2 || i == 1 && used < 1);
}

weight_t counter::operator[](ind_t x) const {
  return cnts[x];
}
