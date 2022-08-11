//
// Created by mavlyut on 19.07.22.
//

#ifndef HUFFMAN_HUFFMAN_H
#define HUFFMAN_HUFFMAN_H

#include "huffman_tree.h"
#include <iostream>
#include <fstream>
#include <filesystem>

/*template <typename code_t = ct_default> requires Is_Code_t<code_t>
std::pair<code_t&, tree<code_t>&> encode(std::string const& str) {
  std::vector<weight_t> weights(MAX_SIZE, 0);
  tree tr(weights);
  for (char tmp_char : str) {
    weights[to_char_t(tmp_char)]++;
  }
  tr.build_tree();
  code_t ans;
  for (char tmp_char : str) {
    ans.push(tr.get_code(to_char_t(tmp_char)));
  }
  return {ans, tr};
}*/

template <typename code_t = huffman_code_type_examples::ct_default>
requires Is_Code_t<code_t>
void encode(char const* in, std::basic_ostream<char>& fout = std::cout) {
  std::ifstream fin;
  fin.open(in, std::fstream::in);
  fin >> std::noskipws;
  tree<code_t> tr;
  char tmp_char;
  while (fin >> tmp_char) {
    tr.inc(to_char_t(tmp_char));
  }
  fin.close();
  // EFO
  if (!tr.get_cnt_used()) {
    return;
  }
  if (tr.get_cnt_used() == 1) {
    fout << "-3\n";
    for (size_t i = 0; i < MAX_SIZE; i++) {
      if (tr.get_count(i)) {
        fout << from_char_t(i) << tr.get_count(i);
        break;
      }
    }
    return;
  }
  tr.build_tree();
  fin.open(in, std::fstream::in);
  fin >> std::noskipws;
  if (tr.need_to_compress(std::filesystem::file_size(in))) {
    fout << tr;
    obstream bout = fout;
    while (fin >> tmp_char) {
      bout << tr.get_code(to_char_t(tmp_char));
    }
    bout.flush();
  } else {
    fout << "-2\n";
    while (fin >> tmp_char) {
      fout << tmp_char;
    }
  }
  fin.close();
}

template <typename code_t = huffman_code_type_examples::ct_default>
requires Is_Code_t<code_t>
void encode(char const* in, char const* out) {
  std::ofstream fout(out, std::fstream::out);
  encode<code_t>(in, fout);
  fout.close();
}

/*template <typename code_t = huffman_code_type_examples::ct_default>
requires Is_Code_t<code_t>
std::string decode(std::pair<code_t, tree<code_t> const&> pr) {
  auto root = pr.second.root();
  auto nd = root;
  std::string ans;
  while (!pr.first.is_empty()) {
    nd = (pr.first.pop_front() ? nd->right : nd->left);
    if (nd->is_leaf()) {
      ans.push_back(from_char_t(nd->value));
      nd = root;
    }
  }
  return ans;
}*/

template <typename code_t = huffman_code_type_examples::ct_default>
requires Is_Code_t<code_t>
static void read_all_codes(ibstream& bin, tree<code_t>& tr) {
  weight_t len;
  for (size_t i = 0; i < MAX_SIZE; i++) {
    bin >> len;
    std::string tmp = read_bin_string(bin, len, len);
    tr.push(tmp, i);
#ifdef LOG
    std::cout << from_char_t(i) << " " << tmp << "\n";
#endif
  }
}

template <typename code_t = huffman_code_type_examples::ct_default>
requires Is_Code_t<code_t>
static void read_used_codes(ibstream& bin, tree<code_t>& tr, size_t cnt) {
  char letter;
  weight_t len;
  std::string tmp;
  for (size_t i = 0; i < cnt; i++) {
    bin >> letter >> len;
    tmp = read_bin_string(bin, len);
    tr.push(tmp, letter);
#ifdef LOG
    std::cout << letter << " " << tmp << "\n";
#endif
  }
}

template <typename code_t = huffman_code_type_examples::ct_default>
requires Is_Code_t<code_t>
void decode(std::basic_istream<char>& fin = std::cin,
            std::basic_ostream<char>& fout = std::cout) {
  tree<code_t> tr;
  ibstream bin = fin;
  char tmp;
  int mode;
  if (!(bin >> mode)) return;
  if (mode == -2) {
    while (fin >> tmp) {
      fout << tmp;
    }
    return;
  }
  if (mode == -3) {
    fin >> tmp >> mode;
    for (size_t i = 0; i < mode; i++) {
      fout << tmp;
    }
    return;
  }
  if (mode < -1 || mode > MAX_SIZE) {
    throw std::runtime_error("File was broken: unknown mode " + std::to_string(mode));
  }
  if (mode == -1) {
    read_all_codes(bin, tr);
  } else {
    read_used_codes(bin, tr, mode);
  }
  bool x;
  auto nd = tr.get_root();
  while (bin) {
    bin >> x;
#ifdef LOG
    std::cout << x;
#endif
    nd = x ? nd->right : nd->left;
    if (nd->is_leaf()) {
      fout << from_char_t(nd->value);
      nd = tr.get_root();
    }
  }
#ifdef LOG
  std::cout << '\n';
#endif
}

template <typename code_t = huffman_code_type_examples::ct_default>
requires Is_Code_t<code_t>
void decode(char const* in, std::basic_ostream<char>& fout = std::cout) {
  std::ifstream fin(in, std::fstream::in);
  decode<code_t>(fin, fout);
  fin.close();
}

template <typename code_t = huffman_code_type_examples::ct_default>
requires Is_Code_t<code_t>
void decode(std::basic_istream<char>* fin, char const* out) {
  std::ofstream fout(out, std::fstream::out);
  decode<code_t>(fin, fout);
  fout.close();
}

template <typename code_t = huffman_code_type_examples::ct_default>
requires Is_Code_t<code_t>
void decode(char const* in, char const* out) {
  std::ofstream fout(out, std::fstream::out);
  decode<code_t>(in, fout);
  fout.close();
}

#endif // HUFFMAN_HUFFMAN_H
