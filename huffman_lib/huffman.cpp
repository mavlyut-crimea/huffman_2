//
// Created by mavlyut on 01/10/22.
//

#include "include/huffman.h"
#include "include/binary_reader.h"
#include "include/binary_writer.h"
#include "include/huffman_tree.h"
#include <fstream>

std::ostream& print(std::ostream& out, ind_t i, std::pair<int_t, len_t> const& p) {
  if (p.second) {
    out << static_cast<char>(i) << ' ';
    for (ind_t j = p.second; j --> 0; ) {
      out << ((p.first >> j) & 1);
    }
    out << '\n';
  }
  return out;
}

static void check_stream(std::istream& in) {
  if (!in.good())
    throw std::runtime_error("Broken file");
}

void encode(char const* in, char const* out) {
  counter cntr;
  cntr.read_from_file(in);
  encode_huffman_tree tr(cntr);
  std::ifstream fin(in, std::ifstream::in);
  check_stream(fin >> std::noskipws);
  std::ofstream fout(out, std::ifstream::out);
  binary_writer bw(fout);
  write(bw, tr);
  char tmp = 0;
  while (fin >> tmp)
    bw.write(tr.get_code(tmp));
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
  while (!br.eof()) {
    bool b = br.next_bool();
    tr.move(tmp_node, b);
//    std::cout << b;
    if (tr.is_leaf(tmp_node)) {
      fout << tr.get_char(tmp_node);
//      std::cout << tr.get_char(tmp_node);
      tmp_node = 0;
    }
  }
}
