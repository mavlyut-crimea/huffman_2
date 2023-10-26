//
// Created by mavlyut on 07/10/22.
//

#include "include/binary_reader.h"

static constexpr std::streamsize UNINITIALIZED = 0xded;

binary_reader::binary_reader(std::istream& in)
    : tmp_char(0), pos(0), in(in >> std::noskipws), rem(-1)
    , len(UNINITIALIZED), cnt(UNINITIALIZED), buf(BUFSIZE, 0), rdbuf(nullptr) {}

binary_reader::~binary_reader() {
  buf.clear();
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
  return pos == 0 && len == 0 && in.eof();
}

void binary_reader::check_buffer() {
  if (len <= cnt) {
    cnt = 0;
    len = rdbuf->sgetn(buf.data(), BUFSIZE);
  }
}

void binary_reader::set_rem() {
  if (!(in >> rem))
    throw std::runtime_error("Error: expected number");
  rdbuf = in.rdbuf();
  len = rdbuf->sgetn(buf.data(), BUFSIZE);
  tmp_char = buf[1];
  cnt = 2;
  pos = (len <= cnt ? rem : BYTESIZE);
  check_buffer();
}

len_t binary_reader::next_int() {
  len_t x = 0;
  char ws = 0;
  in >> x >> ws;
  return x;
}

void binary_reader::read(std::string& str) {
  if (in.rdbuf()->sgetn(str.data(), str.size()) != str.size())
    throw std::runtime_error("Error: incorrect tree");
}
