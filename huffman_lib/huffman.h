//
// Created by mavlyut on 19.07.22.
//

#ifndef HUFFMAN_HUFFMAN_H
#define HUFFMAN_HUFFMAN_H

#include <iostream>
#include <fstream>
#include <filesystem>
#include <sstream>

#include "huffman_tree.h"

static void check_stream(std::basic_ios<char> const& stream) {
  if (!stream.good()) {
    throw std::runtime_error("I/O error while opening stream");
  }
}

template <typename T>
struct hstream {
  static void start_work(T& x, char const* in) {
    x.open(in, std::ios_base::binary | std::ios_base::in);
  }

  static void end_work(T& x) {
    x.close();
  }
};

template <>
struct hstream<std::stringstream> {
  static void start_work(std::stringstream& x, char const* in) {
    x.clear();
    x = std::stringstream(in, std::ios_base::binary | std::ios_base::in);
  }

  static void end_work(std::stringstream& x) {
    x.clear();
  }
};

#define t(num) time_t t##num = std::time(nullptr)

template <typename code_t = huffman_code_type_examples::ct_default,
    bool read_from_file = true,
    typename std::enable_if<std::is_base_of<huffman_code_type, code_t>::value
        && !std::is_same<huffman_code_type, code_t>::value, void*>::type = nullptr>
void encode(char const* in, std::basic_ostream<char>& fout) {
  t(10);
  using stream_type = std::conditional_t<read_from_file, std::ifstream, std::stringstream>;
  using worker = hstream<stream_type>;
  stream_type fin;
  worker::start_work(fin, in);
  check_stream(fin >> std::noskipws);
  tree<code_t> tr;
  char tmp_char;
//  buffered_reader br(fin);
  t(0);
  while (fin >> tmp_char) {
    tr.inc(to_char_t(tmp_char));
  }
#ifdef LOG_CLANG_PROBLEM
  std::cout << "CNT: " << tr.get_cnt_used() << "\n";
#endif
  t(1);
  worker::end_work(fin);
  if (tr.get_cnt_used() == 1) {
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
  worker::start_work(fin, in);
  fin >> std::noskipws;
  check_stream(fin);
  t(2);
  fout << tr;
  t(3);
  obstream bout(fout);
  t(4);
  while (fin >> tmp_char) {
    std::cout << tmp_char;
    tr.get_code(to_char_t(tmp_char)).print_optimized(bout);
  }
  t(5);
  bout.flush();
  worker::end_work(fin);
  t(11);
  std::cout << "Pre-calc: " << t1 - t0 << "s\n";
  std::cout << "Print tree: " << t3 - t2 << "s\n";
  std::cout << "Print enc_file: " << t5 - t4 << "s\n";
  std::cout << "Print vector<ints>: " << huffman_code_type_examples::stime << "s\n";
  std::cout << "All encode: " << t11 - t10 << "s\n";
}

template <typename code_t = huffman_code_type_examples::ct_default,
    typename std::enable_if<std::is_base_of<huffman_code_type, code_t>::value
        && !std::is_same<huffman_code_type, code_t>::value, void*>::type = nullptr>
void encode(char const* in, char const* out) {
  std::ofstream fout(out, std::ios_base::binary | std::ios_base::out);
  check_stream(fout);
  encode<code_t, true>(in, fout);
  fout.close();
}

template <typename code_t = huffman_code_type_examples::ct_default,
    typename std::enable_if<std::is_base_of<huffman_code_type, code_t>::value
        && !std::is_same<huffman_code_type, code_t>::value, void*>::type = nullptr>
std::string encode(std::string const& str) {
  std::stringstream sout(std::ios_base::binary | std::ios_base::out);
  encode<code_t, false>(str.c_str(), sout);
  return sout.str();
}

template <typename code_t = huffman_code_type_examples::ct_default,
    typename std::enable_if<std::is_base_of<huffman_code_type, code_t>::value
        && !std::is_same<huffman_code_type, code_t>::value, void*>::type = nullptr>
static void read_all_codes(ibstream& bin, tree<code_t>& tr) {
  weight_t len;
  std::string tmp;
  for (size_t i = 0; i < MAX_SIZE; i++) {
    bin.read(len);
    tmp = read_bin_string(bin, len, len);
    tr.push(tmp, from_char_t(i));
  }
}

template <typename code_t = huffman_code_type_examples::ct_default,
    typename std::enable_if<std::is_base_of<huffman_code_type, code_t>::value
        && !std::is_same<huffman_code_type, code_t>::value, void*>::type = nullptr>
static void read_used_codes(ibstream& bin, tree<code_t>& tr, size_t cnt) {
  char letter;
  weight_t len;
  std::string tmp;
  for (size_t i = 0; i < cnt; i++) {
    bin.read(letter);
    bin.read(len);
    tmp = read_bin_string(bin, len);
    tr.push(tmp, letter);
  }
}

template <typename code_t = huffman_code_type_examples::ct_default,
    typename std::enable_if<std::is_base_of<huffman_code_type, code_t>::value
        && !std::is_same<huffman_code_type, code_t>::value, void*>::type = nullptr>
void decode(std::basic_istream<char>& fin,
            std::basic_ostream<char>& fout = std::cout) {
  std::cout << "DECODE\n";
  tree<code_t> tr;
  char tmp;
  int imode = MODES::UNKNOWN;
  if (!(fin >> std::noskipws >> imode >> tmp)) {
    throw std::runtime_error("File was broken: no mode");
  }
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
  }
  ibstream bin(fin);
  if (mode == MODES::ALL) {
    read_all_codes(bin, tr);
  } else if (mode == MODES::USED) {
    read_used_codes(bin, tr, imode);
  } else {
    throw std::runtime_error("File was broken: unknown mode " + std::to_string(imode));
  }
  {
    std::string s;
    print(tr.get_root(), s);
  }
  bool x;
  auto nd = tr.get_root();
  while (bin) {
    bin.read(x);
    std::cout << x;
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
    typename std::enable_if<std::is_base_of<huffman_code_type, code_t>::value
        && !std::is_same<huffman_code_type, code_t>::value, void*>::type = nullptr>
void decode(char const* in, std::basic_ostream<char>& fout = std::cout) {
  std::ifstream fin(in, std::ios_base::binary | std::ios_base::in);
  check_stream(fin);
  decode<code_t>(fin, fout);
  fin.close();
}

template <typename code_t = huffman_code_type_examples::ct_default,
    typename std::enable_if<std::is_base_of<huffman_code_type, code_t>::value
        && !std::is_same<huffman_code_type, code_t>::value, void*>::type = nullptr>
void decode(std::basic_istream<char>* fin, char const* out) {
  std::ofstream fout(out, std::ios_base::binary | std::ios_base::out);
  check_stream(fout);
  decode<code_t>(fin, fout);
  fout.close();
}

template <typename code_t = huffman_code_type_examples::ct_default,
    typename std::enable_if<std::is_base_of<huffman_code_type, code_t>::value
        && !std::is_same<huffman_code_type, code_t>::value, void*>::type = nullptr>
void decode(char const* in, char const* out) {
  std::ofstream fout(out, std::ios_base::binary | std::ios_base::out);
  check_stream(fout);
  decode<code_t>(in, fout);
  fout.close();
}

template <typename code_t = huffman_code_type_examples::ct_default,
    typename std::enable_if<std::is_base_of<huffman_code_type, code_t>::value
        && !std::is_same<huffman_code_type, code_t>::value, void*>::type = nullptr>
std::string decode(std::string const& str) {
  std::stringstream sin(str, std::ios_base::binary | std::ios_base::in);
  std::stringstream sout(std::ios_base::binary | std::ios_base::out);
  decode<code_t>(sin, sout);
  return sout.str();
}

#endif // HUFFMAN_HUFFMAN_H
