//
// Created by mavlyut on 02/10/22.
//

#ifndef HUFFMAN_BINARY_WRITER_H
#define HUFFMAN_BINARY_WRITER_H

#include "consts.h"
#include <iostream>

struct binary_writer {
  explicit binary_writer(std::ostream& = std::cout);
  binary_writer(binary_writer const&) = delete;
  binary_writer& operator=(binary_writer const&) = delete;
  ~binary_writer();

  binary_writer& write(int_t);
  void write(code_t const&);
  void flush();

private:
  char tmp_char, pos;
  std::string buf;
  std::ostream& out;

  void check();
};

#endif // HUFFMAN_BINARY_WRITER_H
