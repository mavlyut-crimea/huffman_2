//
// Created by mavlyut on 07/10/22.
//

#include "include/binary_reader.h"

static constexpr std::streamsize UNINITIALIZED = 0xded;

binary_reader::binary_reader(std::istream& in)
    : tmp_char(0), pos(0), in(in), rem(-1)
    , len(UNINITIALIZED), cnt(UNINITIALIZED), buf(BUFSIZE, 0) {}

binary_reader::~binary_reader() = default;

code_t binary_reader::next_code() {
  len_t l = 0;
  int_t c = 0;
  if (!(in >> l))
    throw std::runtime_error("Expected number: length of code");
  if (l != 0)
    if (!(in >> c))
      throw std::runtime_error("Expected number: code");
  return { c, l };
}

bool binary_reader::next_bool() {
  if (pos == 0) {
    tmp_char = buf[cnt++];
    check_buffer();
    pos = (len == 0 ? rem : BYTESIZE);
   }
  bool ans = (tmp_char >> (--pos)) & 1;
  return ans;
}

bool binary_reader::eof() const {
  return pos == 0 && len == 0;
}

void binary_reader::set_rem() {
  std::noskipws(in >> rem);
  len = in.readsome(buf.data(), BUFSIZE);
  tmp_char = buf[1];
  cnt = 2;
  pos = (len <= cnt ? rem : BYTESIZE);
  check_buffer();
}

void binary_reader::check_buffer() {
  if (len <= cnt) {
    cnt = 0;
    len = in.readsome(buf.data(), BUFSIZE);
  }
}
