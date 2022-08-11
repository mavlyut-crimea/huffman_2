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

bool ct_string::is_empty() const {
  return _str.empty();
}

size_t ct_string::size() const {
  return _str.length();
}

bool ct_string::pop_front() {
  bool ans = _str[0] == '1';
  _str.erase(_str.begin());
  return ans;
}

bool ct_string::pop_back() {
  bool ans = _str.back() == '1';
  _str.pop_back();
  return ans;
}

void ct_string::print(std::basic_ostream<char>& out) const {
  out << _str;
}

static char to_digit(char x) {
  return x == '1' ? 1 : 0;
}

static constexpr size_t BLOCK_SIZE = sizeof(char) * 8;

void ct_string::print_optimized(std::basic_ostream<char>& out) const {
  size_t end = _str.length() / BLOCK_SIZE * BLOCK_SIZE;
  char tmp;
  for (size_t i = 0; i < end; i += BLOCK_SIZE) {
    tmp = 0;
    for (size_t j = 0; j < BLOCK_SIZE; j++) {
      tmp <<= 1;
      if (_str[i + j] == '1') tmp++;
    }
    out << tmp;
  }
  if (_str.length() != end) {
    tmp = 0;
    for (size_t i = end; i < _str.length(); i++) {
      tmp <<= 1;
      if (_str[i] == '1')
        tmp++;
    }
    out << tmp;
  }
}

void ct_string::print_optimized(obstream& bout) const {
  for (char i : _str) {
    bout << (i == '1');
  }
}

void ct_string::reset() {
  _str.clear();
}
