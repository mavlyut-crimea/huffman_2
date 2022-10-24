//
// Created by mavlyut on 01/10/22.
//

#ifndef HUFFMAN_COUNTER_H
#define HUFFMAN_COUNTER_H

#include "consts.h"
#include <string>

struct counter {
  counter();
  ~counter();

  void append(char, weight_t = 1);
  void append(std::string const&);
  void read_from_file(char const*);
  void parse_header(char const*);
  weight_t operator[](ind_t) const;

private:
  std::vector<weight_t> cnts;
};

#endif // HUFFMAN_COUNTER_H
