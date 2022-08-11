//
// Created by mavlyut on 24.07.22.
//

#include "../huffman_code.h"
#include <sstream>

static constexpr size_t BLOCK_SIZE = sizeof(char) * 8;

using huffman_code_type_examples::ct_vector_bool;

ct_vector_bool::ct_vector_bool() = default;

ct_vector_bool::~ct_vector_bool() {
  _vec.clear();
}

void ct_vector_bool::push_left_code() {
  _vec.push_back(false);
}

void ct_vector_bool::push_right_code() {
  _vec.push_back(true);
}

bool ct_vector_bool::is_empty() const {
  return _vec.empty();
}

size_t ct_vector_bool::size() const {
  return _vec.size();
}

bool ct_vector_bool::pop_front() {
  bool ans = _vec[0];
  _vec.erase(_vec.begin());
  return ans;
}

bool ct_vector_bool::pop_back() {
  bool ans = _vec.back();
  _vec.pop_back();
  return ans;
}

void ct_vector_bool::print(std::basic_ostream<char>& out) const {
  for (bool i : _vec) {
    out << (i ? '1' : '0');
  }
}

void ct_vector_bool::print_optimized(std::basic_ostream<char>& out) const {
  size_t end = _vec.size() / BLOCK_SIZE * BLOCK_SIZE;
  for (size_t i = 0; i < end; i += BLOCK_SIZE) {
    char tmp = 0;
    for (size_t j = 0; j < BLOCK_SIZE; j++) {
      tmp <<= 1;
      tmp += _vec[i + j];
    }
    out << tmp;
  }
  char tmp = 0;
  for (int i = 0, deg = 1; i < _vec.size() % 4; i++, deg *= 2) {
    tmp += _vec[_vec.size() - 1 - i] * deg;
  }
  out << tmp;
}

void ct_vector_bool::print_optimized(obstream& bout) const {
  for (bool x : _vec) {
    bout << x;
  }
}

void ct_vector_bool::reset() {
  _vec.clear();
}
