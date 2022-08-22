//
// Created by mavlyut on 19.07.22.
//

#ifndef HUFFMAN_HUFFMAN_H
#define HUFFMAN_HUFFMAN_H

#include "huffman_tree.h"
#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>

static void check_stream(std::basic_ios<char> const& stream) {
  if (!stream.good()) {
    throw std::runtime_error("I/O error while opening stream");
  }
}

template <typename T,
    typename std::enable_if_t<!std::is_same_v<T, std::stringstream>, void*> = nullptr>
static void end_work(T& x) {
  x.close();
}

template <typename T,
    typename std::enable_if_t<std::is_same_v<T, std::stringstream>, void*> = nullptr>
static void end_work(T& x) {
  x.clear();
}

template <typename code_t = huffman_code_type_examples::ct_default,
    bool read_from_file = true,
    typename std::enable_if<std::is_base_of_v<huffman_code_type, code_t>
        && !std::is_same_v<huffman_code_type, code_t>, void*>::type = nullptr>
void encode(char const* in, std::basic_ostream<char>& fout) {
  using stream_type = std::conditional_t<read_from_file, std::ifstream, std::stringstream>;
  stream_type fin(in, std::ios_base::in);
  fin >> std::noskipws;
  check_stream(fin);
  tree<code_t> tr;
  char tmp_char;
  while (fin >> tmp_char) {
    tr.inc(to_char_t(tmp_char));
  }
  end_work<stream_type>(fin);
  // EFO
//  if (!tr.get_cnt_used()) {
//    fout << "0";
//    return;
//  }
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
  fin = stream_type(in, std::ios_base::in);
  fin >> std::noskipws;
  check_stream(fin);
  fout << tr;
  obstream bout = fout;
  while (fin >> tmp_char) {
    bout << tr.get_code(to_char_t(tmp_char));
  }
  bout.flush();
  end_work<stream_type>(fin);
}

template <typename code_t = huffman_code_type_examples::ct_default,
    typename std::enable_if<std::is_base_of_v<huffman_code_type, code_t>
        && !std::is_same_v<huffman_code_type, code_t>, void*>::type = nullptr>
void encode(char const* in, char const* out) {
  std::ofstream fout(out, std::ios_base::out);
  check_stream(fout);
  encode<code_t, true>(in, fout);
  fout.close();
}

template <typename code_t = huffman_code_type_examples::ct_default,
    typename std::enable_if<std::is_base_of_v<huffman_code_type, code_t>
        && !std::is_same_v<huffman_code_type, code_t>, void*>::type = nullptr>
std::string encode(std::string const& str) {
  std::stringstream sout(std::ios_base::out);
  encode<code_t, false>(str.c_str(), sout);
  return sout.str();
}

template <typename code_t = huffman_code_type_examples::ct_default,
    typename std::enable_if<std::is_base_of_v<huffman_code_type, code_t>
        && !std::is_same_v<huffman_code_type, code_t>, void*>::type = nullptr>
static void read_all_codes(ibstream& bin, tree<code_t>& tr) {
  weight_t len;
  std::string tmp;
  for (size_t i = 0; i < MAX_SIZE; i++) {
    bin >> len;
    tmp = read_bin_string(bin, len, len);
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
  bin >> imode;
  if (imode == 0) {
    return;
  }
  MODES mode = to_mode(imode);
  if (mode == MODES::ONE_CHAR) {
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
    if ((nd->right == nullptr) != (nd->left == nullptr)) {
      std::cout << "error\n";
    }
    nd = x ? nd->right : nd->left;
    if (nd->is_leaf()) {
      fout << from_char_t(nd->value);
      nd = tr.get_root();
    }
  }
}

template <typename code_t = huffman_code_type_examples::ct_default,
    typename std::enable_if<std::is_base_of_v<huffman_code_type, code_t>
        && !std::is_same_v<huffman_code_type, code_t>, void*>::type = nullptr>
void decode(char const* in, std::basic_ostream<char>& fout = std::cout) {
  std::ifstream fin(in, std::ios_base::in);
  check_stream(fin);
  decode<code_t>(fin, fout);
  fin.close();
}

template <typename code_t = huffman_code_type_examples::ct_default,
    typename std::enable_if<std::is_base_of_v<huffman_code_type, code_t>
        && !std::is_same_v<huffman_code_type, code_t>, void*>::type = nullptr>
void decode(std::basic_istream<char>* fin, char const* out) {
  std::ofstream fout(out, std::ios_base::out);
  check_stream(fout);
  decode<code_t>(fin, fout);
  fout.close();
}

template <typename code_t = huffman_code_type_examples::ct_default,
    typename std::enable_if<std::is_base_of_v<huffman_code_type, code_t>
        && !std::is_same_v<huffman_code_type, code_t>, void*>::type = nullptr>
void decode(char const* in, char const* out) {
  std::ofstream fout(out, std::ios_base::out);
  check_stream(fout);
  decode<code_t>(in, fout);
  fout.close();
}

template <typename code_t = huffman_code_type_examples::ct_default,
    typename std::enable_if<std::is_base_of_v<huffman_code_type, code_t>
        && !std::is_same_v<huffman_code_type, code_t>, void*>::type = nullptr>
std::string decode(std::string const& str) {
  std::stringstream sin(str, std::ios_base::in);
  std::stringstream sout(std::ios_base::out);
  decode<code_t>(sin, sout);
  return sout.str();
}

#endif // HUFFMAN_HUFFMAN_H
