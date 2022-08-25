//
// Created by mavlyut on 03.08.22.
//

#ifndef HUFFMAN_BSTREAM_H
#define HUFFMAN_BSTREAM_H

#include <iostream>
#include <type_traits>
#include <vector>

#define BYTESIZE 8

struct buffered_reader {
  buffered_reader(std::basic_istream<char>& = std::cin);
  buffered_reader(buffered_reader const&) = delete;
  buffered_reader& operator=(buffered_reader const&) = delete;
  ~buffered_reader();

  buffered_reader& operator>>(char&);
  buffered_reader& operator>>(size_t&);
  bool eof() const;
  explicit operator bool() const;

private:
  size_t pos;
  size_t cnt;
  std::vector<char> buf;
  std::basic_istream<char>& in;

  void check();
};

struct ibstream {
  ibstream(std::basic_istream<char>& = std::cin);
  ibstream(ibstream const&) = delete;
  ibstream& operator=(ibstream const&) = delete;
  ~ibstream();

  ibstream& operator>>(char&);
  ibstream& operator>>(bool&);
  ibstream& operator>>(size_t&);
  explicit operator bool() const;

private:
  char tmp_char, next_char;
  size_t mod;
  buffered_reader in;
};

std::string read_bin_string(ibstream&, size_t, bool = false);

struct obstream {
  obstream(std::basic_ostream<char>& = std::cout);
  obstream(obstream const&) = delete;
  obstream& operator=(obstream const&) = delete;
  ~obstream();

  obstream& operator<<(bool);

  template <typename T,
      typename std::enable_if<std::is_integral<T>::value, void*>::type = nullptr>
  void print_int(T const& x) {
    if (mod == BYTESIZE) {
      buf += tmp_char;
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
  std::basic_ostream<char>& out;
  std::string buf;

  void append(char);
};

#endif // HUFFMAN_BSTREAM_H
