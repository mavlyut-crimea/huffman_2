//
// Created by mavlyut on 03.08.22.
//

#include <cstring>
#include <cassert>

#include "bstream.h"

#define NEED_READ 100
#define BUFSIZE 4096
#define BASE 10

void my_assert(bool statement, char msg) {
//   if (!statement)
//     throw std::runtime_error(std::string(1, msg));
}

/// buffered reader

buffered_reader::buffered_reader(std::basic_istream<char>& in)
    : pos(0), cnt(0), buf(BUFSIZE, 0), in(in >> std::noskipws) {
#ifdef LOG_CLANG_PROBLEM
  std::cout << "constructor of buffered_reader" << in.operator bool() << "\n";
#endif
  check();
#ifdef LOG_CLANG_PROBLEM
  std::cout << cnt << "(expect > 0 for non-empty files) \n";
#endif
}

buffered_reader::~buffered_reader() = default;

bool buffered_reader::read(char& x) {
  if (eof()) {
    return false;
  }
  x = buf[pos++];
  check();
  return true;
}

bool buffered_reader::read(size_t& x) {
  x = 0;
  if (buf[pos] < '0' || buf[pos] > '9') {
    return false;
  }
  while (!eof() && buf[pos] >= '0' && buf[pos] <= '9') {
    (x *= BASE) += buf[pos++] - '0';
    check();
  }
  return true;
}

bool buffered_reader::eof() const {
  return cnt == 0;
}

void buffered_reader::check() {
  if (pos == cnt) {
    cnt = in.readsome(buf.data(), BUFSIZE);
#ifdef LOG_CLANG_PROBLEM
    std::cout << "BUF: |";
    for (size_t i = 0; i < cnt; i++) {
      std::cout << buf[i];
    }
    std::cout << "|\n";
#endif
    pos = 0;
  }
}

/// ibstream

ibstream::ibstream(std::basic_istream<char>& in)
    : tmp_char(0), next_char(0), mod(NEED_READ), in(in >> std::noskipws) {}

ibstream::~ibstream() = default;

void ibstream::read(char& x) {
  in.read(x);
}

void ibstream::read(bool& x) {
  if (!mod || mod == NEED_READ) {
    if (mod == NEED_READ) {
      in.read(next_char);
    }
    tmp_char = next_char;
    in.read(next_char);
    if (in.eof()) {
      mod = next_char - '0';
      my_assert(next_char >= '1' && next_char <= '8', next_char);
    } else {
      mod = BYTESIZE;
    }
  }
  my_assert(mod >= 1 && mod <= BYTESIZE, next_char);
  x = (tmp_char >> (--mod)) & 1;
}

void ibstream::read(size_t& x) {
  char ws = 0;
  if (in.read(x)) {
    in.read(ws);
  } else {
    throw std::runtime_error("File was broken: no integers");
  }
}

std::string read_bin_string(ibstream& bin, size_t len, bool need_endl) {
  // len -- length in bits
  std::string ans;
  char tmp = 0;
  for (size_t i = 0; i < len / BYTESIZE; i++) {
    bin.read(tmp);
    for (size_t k = BYTESIZE; k --> 0; ) {
      ans += (tmp >> k) & 1 ? '1' : '0';
    }
  }
  if (len % BYTESIZE) {
    bin.read(tmp);
    for (size_t k = len % BYTESIZE; k --> 0; ) {
      ans += (tmp >> k) & 1 ? '1' : '0';
    }
  }
  if (need_endl) {
    bin.read(tmp);
  }
  return ans;
}

ibstream::operator bool() const {
  return !in.eof() || mod;
}

/// obstream

obstream::obstream(std::basic_ostream<char>& out)
    : mod(0), tmp_char(0), out(out << std::noboolalpha) {}

obstream::~obstream() {
  flush();
}

void obstream::print(bool x) {
  if (mod == BYTESIZE) {
    append(tmp_char);
    tmp_char = 0;
    mod = 0;
  }
  tmp_char <<= 1;
  if (x) {
    tmp_char++;
  }
  mod++;
}

void obstream::flush() {
  if (buf.empty()) {
    return;
  }
  out << buf;
  if (mod) {
    out << tmp_char << mod;
    out.flush();
    tmp_char = 0;
    mod = 0;
  }
  buf.clear();
}

void obstream::append(char ch) {
  buf.push_back(ch);
  if (buf.size() > BUFSIZE) {
    out << buf;
    buf.clear();
  }
}
