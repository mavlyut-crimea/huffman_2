//
// Created by mavlyut on 07/10/22.
//

#ifndef HUFFMAN_BINARY_READER_H
#define HUFFMAN_BINARY_READER_H

#include "consts.h"
#include <iostream>

struct binary_reader {
  explicit binary_reader(std::istream& = std::cin);
  binary_reader(binary_reader const&) = delete;
  binary_reader& operator=(binary_reader const&) = delete;
  ~binary_reader();

  code_t next_code();
  bool next_bool();
  bool eof() const;
  void set_rem();

private:
  char tmp_char, pos;
  ind_t rem;
  std::streamsize cnt, len;
  std::string buf;
  std::istream& in;

  void check();
  void check_buffer();
};

#endif // HUFFMAN_BINARY_READER_H
