//
// Created by mavlyut on 25.08.22.
//

#ifndef HUFFMAN_BUFFERED_H
#define HUFFMAN_BUFFERED_H

#include <iostream>
#include <cstdint>

namespace buffered {
  struct reader {
    reader(std::basic_istream<char>&);
    ~reader();
    bool read(char&);
    bool read(size_t&);
    bool eof();
    bool operator!() const;

  private:
    char* buf;
    size_t pos = 0, sz = 0;
    std::basic_istream<char>& in;
  };

  struct writer {
    writer(std::basic_ostream<char>&);
    ~writer();
    void write(char);
    void write(size_t);
    void write(std::string const& s);
    bool operator!() const;

  private:
    char* buf;
    size_t pos = 0;
    std::basic_ostream<char>& out;
  };
}

#endif // HUFFMAN_BUFFERED_H
