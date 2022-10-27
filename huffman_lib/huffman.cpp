//
// Created by mavlyut on 01/10/22.
//

#include "include/huffman.h"
#include "include/binary_reader.h"
#include "include/binary_writer.h"
#include "include/huffman_tree.h"
#include <fstream>

static void check_stream(std::istream& in) {
  if (!in.good())
    throw std::runtime_error("Broken file");
}

void encode(char const* in, char const* out) {
  counter cntr;
  cntr.read_from_file(in);
  encode_huffman_tree tr(cntr);
  std::ifstream fin(in, std::ifstream::in);
  check_stream(fin);
  std::ofstream fout(out, std::ifstream::out);
  binary_writer bw(fout);
  write(bw, tr);
  auto rdbuf = fin.rdbuf();
  std::array<char, BUFSIZE> buf{};
  std::streamsize sz = -1;
  while (sz != 0) {
    sz = rdbuf->sgetn(buf.data(), BUFSIZE);
    for (size_t i = 0; i < sz; i++)
      bw.write(tr.get_code(buf[i]));
  }
  bw.flush();
  fin.close();
}

void decode(char const* in, char const* out) {
  std::ifstream fin(in, std::ofstream::in);
  check_stream(fin);
  std::ofstream fout(out, std::ofstream::out);
  binary_reader br(fin);
  decode_huffman_tree tr(br);
  ind_t tmp_node = 0;
  std::string buf(BUFSIZE, 0);
  size_t pos = 0;
  while (!br.eof()) {
    bool b = br.next_bool();
    tr.move(tmp_node, b);
    if (tr.is_leaf(tmp_node)) {
      buf[pos++] = tr.get_char(tmp_node);
      if (pos == BUFSIZE) {
        fout << buf;
        pos = 0;
      }
      tmp_node = 0;
    }
  }
  for (size_t i = 0; i < pos; i++)
    fout << buf[i];
}
