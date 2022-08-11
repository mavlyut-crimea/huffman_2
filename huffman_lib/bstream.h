//
// Created by mavlyut on 03.08.22.
//

#ifndef HUFFMAN_BSTREAM_H
#define HUFFMAN_BSTREAM_H

#include <iostream>

struct ibstream {
  ibstream(std::basic_istream<char>& = std::cin);
  ibstream(ibstream const&) = delete;
  ibstream& operator=(ibstream const&) = delete;
  ~ibstream();

  ibstream& operator>>(char&);
  ibstream& operator>>(bool&);
  ibstream& operator>>(size_t&);
  ibstream& operator>>(int&);
  std::basic_istream<char>& get_istream() const;
  explicit operator bool() const;
  bool operator!() const;

private:
  char tmp_char, next_char, next_next_char;
  bool eof, last_byte;
  size_t mod;
  std::basic_istream<char>& in;
};

std::string read_bin_string(ibstream&, size_t, bool = false);

struct obstream {
  obstream(std::basic_ostream<char>& = std::cout);
  obstream(obstream const&) = delete;
  obstream& operator=(obstream const&) = delete;
  ~obstream();

  obstream& operator<<(bool);
  void flush(); // finish entering binary_string
  std::basic_ostream<char>& get_ostream() const;

private:
  size_t mod;
  char tmp_char;
  std::basic_ostream<char>& out;
  std::string buf;

  void reset();
};

#endif // HUFFMAN_BSTREAM_H
