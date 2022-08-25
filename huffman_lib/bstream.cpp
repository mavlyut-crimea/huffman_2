//
// Created by mavlyut on 03.08.22.
//

#include "bstream.h"

#define NEED_READ 100
#define BUFSIZE BUFSIZ

/// buffered reader
buffered_reader::buffered_reader(std::basic_istream<char>& in)
    : pos(0), cnt(0), buf(BUFSIZE), in(in >> std::noskipws) {
  check();
}

buffered_reader::~buffered_reader() = default;

buffered_reader& buffered_reader::operator>>(char& x) {
  x = buf[pos++];
  check();
  return *this;
}

buffered_reader& buffered_reader::operator>>(size_t& x) {
  x = 0;
  if (buf[pos] < '0' || buf[pos] > '9') {
    throw std::runtime_error("can't parse empty string");
  }
  char tmp;
  while (!eof() && buf[pos] >= '0' && buf[pos] <= '9') {
    *this >> tmp;
    (x *= 10) += tmp - '0';
  }
  return *this;
}

bool buffered_reader::eof() const {
  return cnt == 0;
}

buffered_reader::operator bool() const {
  return in.operator bool();
}

void buffered_reader::check() {
  if (pos == cnt) {
    cnt = in.readsome(&buf[0], BUFSIZE);
    pos = 0;
  }
}

/// ibstream

ibstream::ibstream(std::basic_istream<char>& in)
    : eof(false), last_byte(false), mod(NEED_READ), in(in >> std::noskipws) {}

ibstream::~ibstream() = default;

ibstream& ibstream::operator>>(char& x) {
  in >> x;
  if (in.eof()) {
    eof = true;
  }
  return *this;
}

ibstream& ibstream::operator>>(bool& x) {
  if (!mod || mod == NEED_READ) {
    if (mod == NEED_READ) {
      in >> next_char;
    }
    tmp_char = next_char;
    in >> next_char;
    if (in.eof()) {
      eof = true;
      last_byte = true;
      mod = next_char - '0';
    } else {
      mod = BYTESIZE;
    }
  }
  x = (tmp_char >> (--mod)) & 1;
  return *this;
}

ibstream& ibstream::operator>>(size_t& x) {
  char ws;
  if (in >> x) {
    in >> ws;
  } else {
    throw std::runtime_error("File was broken: no mode");
  }
  if (in.eof()) {
    eof = true;
  }
  return *this;
}

std::string read_bin_string(ibstream& bin, size_t len, bool need_endl) {
  // len -- length in bits
  std::string ans;
  char tmp;
  for (size_t i = 0; i < len / BYTESIZE; i++) {
    bin >> tmp;
    for (size_t k = BYTESIZE; k --> 0; ) {
      ans += (tmp >> k) & 1 ? '1' : '0';
    }
  }
  if (len % BYTESIZE) {
    bin >> tmp;
    for (size_t k = len % BYTESIZE; k --> 0; ) {
      ans += (tmp >> k) & 1 ? '1' : '0';
    }
  }
  if (need_endl) {
    bin >> tmp;
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

obstream& obstream::operator<<(bool x) {
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
  return *this;
}

void obstream::flush() {
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
  buf += ch;
  if (buf.size() > BUFSIZE) {
    out << buf;
    buf.clear();
  }
}
