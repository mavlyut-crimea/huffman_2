//
// Created by mavlyut on 02/10/22.
//

#include "include/binary_writer.h"

binary_writer::binary_writer(std::ostream& out)
    : tmp_char(0), pos(0), out(out) {}

binary_writer::~binary_writer() {
  if (pos != 0)
    buf.push_back(tmp_char);
  flush();
}

void binary_writer::write(len_t l) {
  out << l << ' ';
}

void binary_writer::write(ind_t x) {
  out << x << ' ';
}

void binary_writer::write(code_t const& x) {
  if (x.second == 0)
    return;
  if (pos + x.second > BYTESIZE) {
    size_t i = x.second - BYTESIZE + pos;
    buf.push_back((tmp_char << (BYTESIZE - pos)) + (x.first >> i));
    while (i >= BYTESIZE) {
      i -= BYTESIZE;
      buf.push_back((x.first >> i) & BYTEMASK);
    }
    tmp_char = (((x.first << (BYTESIZE - i)) & BYTEMASK) >> (BYTESIZE - i));
    pos = i;
  } else {
    (tmp_char <<= x.second) += x.first;
    pos += x.second;
  }
  check();
}

void binary_writer::flush() {
  out << buf;
  buf.clear();
}

void binary_writer::check() {
  if (pos == BYTESIZE) {
    buf.push_back(tmp_char);
    pos = 0;
  }
  if (buf.size() > BUFSIZE)
    flush();
}
