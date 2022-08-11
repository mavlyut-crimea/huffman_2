//
// Created by mavlyut on 03.08.22.
//

#include "bstream.h"

/// ibstream

#define NEED_READ 100
#define BUFSIZE 4096
#define BYTESIZE 8

ibstream::ibstream(std::basic_istream<char>& in)
    : eof(false), last_byte(false), mod(NEED_READ), in(in >> std::noskipws) {}

ibstream::~ibstream() = default;

ibstream& ibstream::operator>>(char& x) {
  in >> x;
  if (in) eof = true;
  return *this;
}

ibstream& ibstream::operator>>(bool& x) {
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
  return *this;
}

ibstream& ibstream::operator>>(int& x) {
  // for entering mode
  char ws;
  in >> x >> ws;
  if (in.eof()) eof = true;
  return *this;
}

ibstream& ibstream::operator>>(size_t& x) {
  char ws;
  in >> x >> ws;
  if (in.eof()) eof = true;
  return *this;
}

std::string read_bin_string(ibstream& bin, size_t len, bool need_endl) {
  // len -- length in bits
  std::string ans;
  char tmp;
  for (size_t i = 0; i < len / BYTESIZE; i++) {
    bin >> tmp;
    for (size_t k = BYTESIZE; k-- > 0; ) {
      ans += (tmp >> k) & 1 ? '1' : '0';
    }
  }
  if (len % BYTESIZE) {
    bin >> tmp;
    for (size_t k = len % BYTESIZE; k-- > 0; ) {
      ans += (tmp >> k) & 1 ? '1' : '0';
    }
  }
  if (need_endl) bin >> tmp;
  return ans;
}

std::basic_istream<char>& ibstream::get_istream() const {
  return in;
}

ibstream::operator bool() const {
  return !eof || !last_byte || mod;
}

bool ibstream::operator!() const {
  return eof && mod == NEED_READ;
}

/// obstream

obstream::obstream(std::basic_ostream<char>& out)
    : mod(0), tmp_char(0), out(out << std::noboolalpha) {}

obstream::~obstream() {
  flush();
}

obstream& obstream::operator<<(bool x) {
  if (mod == BYTESIZE) {
    if (buf.size() + 1 > BUFSIZE) {
      out << buf;
      buf.clear();
    }
    reset();
  }
  tmp_char <<= 1;
  if (x) tmp_char++;
  mod++;
  return *this;
}

void obstream::flush() {
  out << buf;
  if (mod) {
    out << tmp_char << mod;
    out.flush();
    reset();
  }
  buf.clear();
}

std::basic_ostream<char>& obstream::get_ostream() const {
  return out;
}

void obstream::reset() {
  buf += tmp_char;
  tmp_char = 0;
  mod = 0;
}
