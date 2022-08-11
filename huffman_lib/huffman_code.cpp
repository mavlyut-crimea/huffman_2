//
// Created by mavlyut on 24.07.22.
//

#include "huffman_code.h"
#include <sstream>

void huffman_code_type::push_code(bool is_right) {
  if (is_right) {
    push_right_code();
  } else {
    push_left_code();
  }
}

std::basic_ostream<char>&
operator<<(std::basic_ostream<char>& out, huffman_code_type const& a) {
  out << a.size();
  if (a.size()) {
    out << ' ';
    a.print_optimized(out);
  }
  return out;
}

std::string huffman_code_type::to_string() const {
  std::stringstream ss;
  print(ss);
  return ss.str();
}

template <typename T>
requires Is_Code_t<T>
T& copy_from_string(std::string const& str) {
  T ans;
  for (char i : str) {
    if (i == '1' || i == '0') {
      ans.push_code(i == '1');
    } else {
      break;
    }
  }
  return ans;
}

obstream& operator<<(obstream& bout, huffman_code_type const& x) {
  x.print_optimized(bout);
  return bout;
}
