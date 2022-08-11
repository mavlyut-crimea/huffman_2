//
// Created by mavlyut on 24.07.22.
//

#include "../huffman_code.h"
#include <sstream>

// FIXME PLEASE, FIXME PLEASE, FIXME PLEASE, FIXME PLEASE, FIXME PLEASE

using huffman_code_type_examples::ct_vector_int;

ct_vector_int::ct_vector_int() : pos_start(0), pos_end(_bitness) {}

ct_vector_int::~ct_vector_int()  {
  _vec.clear();
}

void ct_vector_int::push_left_code()  {
  if (pos_end == _bitness) {
    _vec.push_back(0);
    pos_end = 0;
  }
  _vec.back() <<= 1;
  pos_end++;
}

void ct_vector_int::push_right_code()  {
  push_left_code();
  _vec.back()++;
}

bool ct_vector_int::is_empty() const  {
  return _vec.empty() || (_vec.size() == 1 && pos_start == pos_end);
}

size_t huffman_code_type_examples::ct_vector_int::size() const {
  if (_vec.empty()) return 0;
  return (_vec.size() - 1) * _bitness - pos_start + pos_end;
}

bool ct_vector_int::pop_front()  {
  bool ans = (_vec[0] >> (_vec.size() == 1 ? pos_start : _bitness - pos_start - 1)) & 1;
  if (++pos_start == _bitness) {
    _vec.erase(_vec.begin());
    pos_start = 0;
  }
  return ans;
}

bool ct_vector_int::pop_back() {
  bool ans = _vec.back() & 1;
  if (!--pos_end) {
    _vec.pop_back();
    pos_end = _bitness;
  } else {
    _vec.back() >>= 1;
  }
  return ans;
}

void ct_vector_int::print(std::basic_ostream<char>& out) const {
  print_optimized(out);
}

void ct_vector_int::print_optimized(std::basic_ostream<char>& out) const {
  obstream bout = out;
  print_optimized(bout);
}

// FIXME
void ct_vector_int::print_optimized(obstream& bout) const {
  if (!_vec.empty()) {
    for (size_t j = _vec.size() == 1 ? pos_end : _bitness; j-- > pos_start;) {
      bout << ((_vec.front() >> j) & 1);
    }
    for (size_t i = 1; i < _vec.size() - 1; i++) {
      for (size_t j = _bitness; j-- > 0;) {
        bout << ((_vec[i] >> j) & 1);
      }
    }
  }
  if (_vec.size() - 1) {
    for (size_t j = pos_end; j-- > 0; ) {
      bout << ((_vec.back() >> j) & 1);
    }
  }
}

void ct_vector_int::reset() {
  _vec.clear();
  pos_start = 0;
  pos_end = _bitness;
}
