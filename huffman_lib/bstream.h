//
// Created by mavlyut on 03.08.22.
//

#ifndef HUFFMAN_BSTREAM_H
#define HUFFMAN_BSTREAM_H

#include <iostream>
#include <type_traits>

#define BYTESIZE 8

struct ibstream {
  ibstream(std::basic_istream<char>& = std::cin);
  ibstream(ibstream const&) = delete;
  ibstream& operator=(ibstream const&) = delete;
  ~ibstream();

  void read(char&);
  void read(bool&);
  void read(size_t&);
  explicit operator bool() const;

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

  void print(bool);

  template <typename T,
      typename std::enable_if<std::is_integral<T>::value, void*>::type = nullptr>
  void print_int(T const& x) {
    if (mod == BYTESIZE) {
      buf.push_back(tmp_char);
      mod = 0;
    }
    size_t bitcnt = sizeof(T) * BYTESIZE;
    tmp_char <<= (BYTESIZE - mod);
    tmp_char += (x >> (bitcnt - BYTESIZE + mod));
    if (sizeof(T) == 1 && mod == 0) {
      mod = BYTESIZE;
      return;
    }
    append(tmp_char);
    for (size_t i = BYTESIZE - mod; i + BYTESIZE < bitcnt - 1; i += BYTESIZE) {
      append((x >> (bitcnt - i - BYTESIZE)) & ((1 << BYTESIZE) - 1));
    }
    if (!mod) {
      mod = BYTESIZE;
    }
    tmp_char = (x & ((1 << mod) - 1));
  }

  void flush(); // finish printing binary_string

private:
  size_t mod;
  char tmp_char;
  std::string buf;
  std::basic_ostream<char>& out;

  void append(char);
};

#endif // HUFFMAN_BSTREAM_H
