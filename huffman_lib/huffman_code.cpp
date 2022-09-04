//
// Created by mavlyut on 24.07.22.
//

#include <sstream>

#include "huffman_code.h"

void print(std::basic_ostream<char>& out, huffman_code_type const& a) {
  out << a.size();
  std::cout << a.size() << " ";
  if (a.size()) {
    out << ' ';
    a.print(out);
  }
}

void huffman_code_type::print(std::basic_ostream<char>& out) const {
  size_t sz = size();
  size_t end = sz / BYTESIZE * BYTESIZE;
  char tmp;
  for (size_t i = 0; i < end; i += BYTESIZE) {
    tmp = 0;
    for (size_t j = 0; j < BYTESIZE; j++) {
      tmp <<= 1;
      if (operator[](i + j)) {
        tmp++;
        std::cout << "1";
      } else
        std::cout << "0";
    }
    out << tmp;
  }
  if (sz != end) {
    tmp = 0;
    for (size_t i = end; i < sz; i++) {
      tmp <<= 1;
      if (operator[](i)) {
        tmp++;
        std::cout << "1";
      } else
        std::cout << "0";
    }
    out << tmp;
  }
}

void huffman_code_type::print(obstream& bout) const {
  for (size_t i = 0; i < size(); i++) {
    bout.print(operator[](i));
  }
}
