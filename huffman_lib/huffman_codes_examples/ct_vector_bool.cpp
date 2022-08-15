//
// Created by mavlyut on 24.07.22.
//

#include "../huffman_code.h"
#include <sstream>

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

size_t ct_vector_bool::size() const {
  return _vec.size();
}

bool ct_vector_bool::pop_back() {
  bool ans = _vec.back();
  _vec.pop_back();
  return ans;
}

bool ct_vector_bool::operator[](size_t i) const {
  return _vec[i];
}
