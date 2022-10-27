//
// Created by mavlyut on 27/10/22.
//

#include "include/binary_reader.h"

static constexpr std::streamsize UNINITIALIZED = 0xded;

binary_reader::binary_reader(std::istream& in)
    : tmp_char(0), pos(0), in(in), rem(-1)
    , len(UNINITIALIZED), cnt(UNINITIALIZED), buf(BUFSIZE, 0), rdbuf(nullptr) {}

binary_reader::~binary_reader() {
  buf.clear();
}

code_t binary_reader::next_code(len_t l) {
  int_t c = 0;
  for (len_t i = 0; i < l; i++)
    (c <<= 1) += next_bool();
  return { c, l };
}

void binary_reader::next_len(len_t& l) {
  if (!(in >> l))
    throw std::runtime_error("Expected number");
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
  rdbuf = in.rdbuf();
  len = rdbuf->sgetn(buf.data(), BUFSIZE);
  tmp_char = buf[1];
  cnt = 2;
  pos = (len <= cnt ? rem : BYTESIZE);
  check_buffer();
}

void binary_reader::check_buffer() {
  if (len <= cnt) {
    cnt = 0;
    len = rdbuf->sgetn(buf.data(), BUFSIZE);
  }
}
