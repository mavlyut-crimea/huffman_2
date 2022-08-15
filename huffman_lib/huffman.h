//
// Created by mavlyut on 19.07.22.
//

#ifndef HUFFMAN_HUFFMAN_H
#define HUFFMAN_HUFFMAN_H

#include "huffman_tree.h"
#include <iostream>
#include <fstream>
#include <filesystem>

static void check_stream(std::basic_ios<char> const& stream) {
  if (!stream.good()) {
    throw std::runtime_error("I/O error while opening stream\n");
  }
}

template <typename code_t = huffman_code_type_examples::ct_default,
    typename std::enable_if<std::is_base_of_v<huffman_code_type, code_t>
                            && !std::is_same_v<huffman_code_type, code_t>, void*>::type = nullptr>
void encode(char const* in, std::basic_ostream<char>& fout) {
  std::ifstream fin;
  fin.open(in, std::fstream::in);
  fin >> std::noskipws;
  check_stream(fin);
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
#ifdef LOG
    std::cout << "mode: " << MODES::ONE_CHAR << '\n';
#endif
    fout << MODES::ONE_CHAR << '\n';
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
  check_stream(fin);
  if (tr.need_to_compress(std::filesystem::file_size(in))) {
    fout << tr;
    obstream bout = fout;
    while (fin >> tmp_char) {
      bout << tr.get_code(to_char_t(tmp_char));
    }
    bout.flush();
  } else {
#ifdef LOG
    std::cout << "mode: " << MODES::ORIG << '\n';
#endif
    fout << MODES::ORIG << '\n';
    while (fin >> tmp_char) {
      fout << tmp_char;
    }
  }
  fin.close();
}

template <typename code_t = huffman_code_type_examples::ct_default,
    typename std::enable_if<std::is_base_of_v<huffman_code_type, code_t>
                            && !std::is_same_v<huffman_code_type, code_t>, void*>::type = nullptr>
void encode(char const* in, char const* out) {
  std::ofstream fout(out, std::fstream::out);
  check_stream(fout);
  encode<code_t>(in, fout);
  fout.close();
}

template <typename code_t = huffman_code_type_examples::ct_default,
    typename std::enable_if<std::is_base_of_v<huffman_code_type, code_t>
                            && !std::is_same_v<huffman_code_type, code_t>, void*>::type = nullptr>
std::string encode(std::string const& str) {
  std::stringstream sin(str);
  std::stringstream sout(str);
  encode<code_t>(sin, sout);
  return sout.str();
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

template <typename code_t = huffman_code_type_examples::ct_default,
    typename std::enable_if<std::is_base_of_v<huffman_code_type, code_t>
                            && !std::is_same_v<huffman_code_type, code_t>, void*>::type = nullptr>
static void read_all_codes(ibstream& bin, tree<code_t>& tr) {
  weight_t len;
  for (size_t i = 0; i < MAX_SIZE; i++) {
    bin >> len;
    std::string tmp = read_bin_string(bin, len, len);
    tr.push(tmp, from_char_t(i));
#ifdef LOG
    std::cout << from_char_t(i) << " " << tmp << "\n";
#endif
  }
}

template <typename code_t = huffman_code_type_examples::ct_default,
    typename std::enable_if<std::is_base_of_v<huffman_code_type, code_t>
                            && !std::is_same_v<huffman_code_type, code_t>, void*>::type = nullptr>
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

template <typename code_t = huffman_code_type_examples::ct_default,
    typename std::enable_if<std::is_base_of_v<huffman_code_type, code_t>
                            && !std::is_same_v<huffman_code_type, code_t>, void*>::type = nullptr>
void decode(std::basic_istream<char>& fin,
            std::basic_ostream<char>& fout = std::cout) {
  tree<code_t> tr;
  ibstream bin = fin;
  char tmp;
  int imode;
  if (!(bin >> imode)) return;
  MODES mode = to_mode(imode);
#ifdef LOG
  std::cout << imode << "\n";
#endif
  if (mode == MODES::ORIG) {
    while (fin >> tmp) {
      fout << tmp;
    }
    return;
  } else if (mode == MODES::ONE_CHAR) {
    weight_t cnt;
    fin >> tmp >> cnt;
    for (size_t i = 0; i < cnt; i++) {
      fout << tmp;
    }
    return;
  } else if (mode == MODES::ALL) {
    read_all_codes(bin, tr);
  } else if (mode == MODES::USED) {
    read_used_codes(bin, tr, imode);
  } else {
    throw std::runtime_error("File was broken: unknown imode " + std::to_string(imode));
  }
  bool x;
  auto nd = tr.get_root();
  while (bin) {
    bin >> x;
#ifdef LOG
    std::cout << x;
#endif
    if ((nd->right == nullptr) != (nd->left == nullptr)) {
      std::cout << "error\n";
    }
    nd = x ? nd->right : nd->left;
    if (nd->is_leaf()) {
#ifdef LOG
      std::cout << from_char_t(nd->value);
#endif
      fout << from_char_t(nd->value);
      nd = tr.get_root();
    }
  }
#ifdef LOG
  std::cout << '\n';
#endif
}

template <typename code_t = huffman_code_type_examples::ct_default,
    typename std::enable_if<std::is_base_of_v<huffman_code_type, code_t>
                            && !std::is_same_v<huffman_code_type, code_t>, void*>::type = nullptr>
void decode(char const* in, std::basic_ostream<char>& fout = std::cout) {
  std::ifstream fin(in, std::fstream::in);
  check_stream(fin);
  decode<code_t>(fin, fout);
  fin.close();
}

template <typename code_t = huffman_code_type_examples::ct_default,
    typename std::enable_if<std::is_base_of_v<huffman_code_type, code_t>
                            && !std::is_same_v<huffman_code_type, code_t>, void*>::type = nullptr>
void decode(std::basic_istream<char>* fin, char const* out) {
  std::ofstream fout(out, std::fstream::out);
  check_stream(fout);
  decode<code_t>(fin, fout);
  fout.close();
}

template <typename code_t = huffman_code_type_examples::ct_default,
    typename std::enable_if<std::is_base_of_v<huffman_code_type, code_t>
                            && !std::is_same_v<huffman_code_type, code_t>, void*>::type = nullptr>
void decode(char const* in, char const* out) {
  std::ofstream fout(out, std::fstream::out);
  check_stream(fout);
  decode<code_t>(in, fout);
  fout.close();
}

#endif // HUFFMAN_HUFFMAN_H
