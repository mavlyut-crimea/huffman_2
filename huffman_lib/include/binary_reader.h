//
// Created by mavlyut on 07/10/22.
//

#ifndef HUFFMAN_BINARY_READER_H
#define HUFFMAN_BINARY_READER_H

#include "consts.h"
#include <iostream>

struct binary_reader {
  friend struct decode_huffman_tree;

  explicit binary_reader(std::istream& = std::cin);
  binary_reader(binary_reader const&) = delete;
  binary_reader& operator=(binary_reader const&) = delete;
  ~binary_reader();

  bool next_bool();
  bool eof() const;

private:
  char tmp_char, pos;
  ind_t rem;
  std::streamsize cnt, len;
  std::string buf;
  std::istream& in;
  std::basic_streambuf<char, std::char_traits<char>>* rdbuf;

  void check_buffer();
  void set_rem();
  len_t next_int();
  void read(std::string&);
};

#endif // HUFFMAN_BINARY_READER_H
