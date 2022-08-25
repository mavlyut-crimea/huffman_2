//
// Created by mavlyut on 24.07.22.
//

#include <sstream>

#include "huffman_code.h"

std::basic_ostream<char>&
operator<<(std::basic_ostream<char>& out, huffman_code_type const& a) {
  out << a.size();
  if (a.size()) {
    out << ' ';
    a.print(out);
  }
  return out;
}

void huffman_code_type::print(std::basic_ostream<char>& out) const {
  size_t sz = size();
  size_t end = sz / BYTESIZE * BYTESIZE;
  char tmp;
  for (size_t i = 0; i < end; i += BYTESIZE) {
    tmp = 0;
    for (size_t j = 0; j < BYTESIZE; j++) {
      tmp <<= 1;
      if (operator[](i + j)) tmp++;
    }
    out << tmp;
  }
  if (sz != end) {
    tmp = 0;
    for (size_t i = end; i < sz; i++) {
      tmp <<= 1;
      if (operator[](i)) tmp++;
    }
    out << tmp;
  }
}

void huffman_code_type::print(obstream& bout) const {
  for (size_t i = 0; i < size(); i++) {
    bout << operator[](i);
  }
}

obstream& operator<<(obstream& bout, huffman_code_type const& x) {
  x.print_optimized(bout);
//  x.print(bout);
  return bout;
}
