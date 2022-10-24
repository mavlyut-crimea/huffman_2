//
// Created by mavlyut on 01/10/22.
//

#include "include/counter.h"
#include <array>
#include <fstream>

counter::counter() : cnts(ALPHABET_SIZE, 0) {}

counter::~counter() = default;

void counter::append(char x, weight_t sz) {
  cnts[static_cast<char_t>(x + ALPHABET_SIZE / 2)] += sz;
}

void counter::append(std::string const& s) {
  for (char i : s)
    append(i);
}

void counter::read_from_file(char const* in) {
  std::ifstream fin(in, std::ifstream::in);
  std::array<char, BUFSIZE> buf{};
  std::streamsize sz = -1;
  while (sz != 0) {
    sz = fin.readsome(buf.data(), BUFSIZE);
    for (size_t i = 0; i < sz; i++) {
      append(buf[i]);
    }
  }
  fin.close();
}

void counter::parse_header(char const* in) {
  std::ifstream fin(in, std::ifstream::in);
  for (ind_t i = 0; i < ALPHABET_SIZE; i++)
    fin >> cnts[i];
}

weight_t counter::operator[](ind_t x) const {
  return cnts[x];
}
