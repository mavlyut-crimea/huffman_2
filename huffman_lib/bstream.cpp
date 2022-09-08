//
// Created by mavlyut on 03.08.22.
//

#include "bstream.h"

#define NEED_READ 100
#define BUFSIZE 4096

/// ibstream

ibstream::ibstream(std::basic_istream<char>& in)
    : eof(false), last_byte(false), mod(NEED_READ), in(in >> std::noskipws) {}

ibstream::~ibstream() = default;

void ibstream::read(char& x) {
  in >> x;
  if (in.eof()) {
    eof = true;
  }
}

void ibstream::read(bool& x) {
  if (!mod || mod == NEED_READ) {
    if (mod == NEED_READ) {
      in >> next_char >> next_next_char;
    }
    tmp_char = next_char;
    next_char = next_next_char;
    in >> next_next_char;
    if (in.eof()) {
      eof = true;
      last_byte = true;
      mod = next_next_char - '0';
    } else {
      mod = BYTESIZE;
    }
  }
  x = (tmp_char >> (--mod)) & 1;
}

void ibstream::read(size_t& x) {
  char ws;
  if (in >> x) {
    in >> ws;
  } else {
    throw std::runtime_error("File was broken: no integers");
  }
  if (in.eof()) {
    eof = true;
  }
}

std::string read_bin_string(ibstream& bin, size_t len, bool need_endl) {
  // len -- length in bits
  std::string ans;
  char tmp;
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
    // TODO: assert: tmp == '\n'
  }
  return ans;
}

ibstream::operator bool() const {
  return !eof || !last_byte || mod;
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
  if (buf.empty())
    return;
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
