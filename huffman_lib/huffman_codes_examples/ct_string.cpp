//
// Created by mavlyut on 24.07.22.
//

#include "../huffman_code.h"

using huffman_code_type_examples::ct_string;

ct_string::ct_string() = default;

ct_string::~ct_string() = default;

void ct_string::push_left_code() {
  _str.push_back('0');
}

void ct_string::push_right_code() {
  _str.push_back('1');
}

size_t ct_string::size() const {
  return _str.length();
}

bool ct_string::pop_back() {
  bool ans = _str.back() == '1';
  _str.pop_back();
  return ans;
}

bool ct_string::operator[](size_t i) const {
  return _str[i] == '1';
}

void ct_string::print_optimized(obstream& bout) const {
  print(bout);
}
