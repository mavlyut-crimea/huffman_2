//
// Created by mavlyut on 25.08.22.
//

#include <cstring>

#include "buffered.h"

#define BUFSIZE BUFSIZ

buffered::reader::reader(std::basic_istream<char>& in)
    : buf(new char[BUFSIZE]), in(in) {}

buffered::reader::~reader() {
  delete[] buf;
}

bool buffered::reader::read(char& x) {
  if (pos == sz) {
    if (in.eof()) {
      return false;
    }
    pos = 0;
    sz = in.readsome(buf, BUFSIZE);
  }
  x = buf[pos++];
  return true;
}

bool buffered::reader::read(size_t& x) {
  x = 0;
  char tmp = 0;
  read(tmp);
  if (tmp < '0' || tmp > '9') {
    return false;
  }
  while (tmp >= '0' && tmp <= '9') {
    (x *= 10) += tmp - '0';
    read(tmp);
  }
  return true;
}

bool buffered::reader::eof() {
  return sz == pos && in.eof();
}

bool buffered::reader::operator!() const {
  return !in;
}

buffered::writer::writer(std::basic_ostream<char>& out)
    : buf(new char[BUFSIZE]), out(out) {}

buffered::writer::~writer() {
  delete[] buf;
}

void buffered::writer::write(char x) {
  if (pos == BUFSIZE) {
    out.write(buf, BUFSIZE);
    pos = 0;
  }
  buf[pos++] = x;
}

void buffered::writer::write(size_t x) {
  return write(std::to_string(x));
}

void buffered::writer::write(std::string const& s) {
  if (pos + s.length() >= BUFSIZE) {
    out.write(buf, pos);
    out.write(s.c_str(), s.length());
  }
  strcat(buf, s.c_str());
  pos += s.length();
}

bool buffered::writer::operator!() const {
  return !out;
}
