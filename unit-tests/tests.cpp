#include "../huffman_lib/include/consts.h"
#include "../huffman_lib/include/huffman.h"
#include "gtest/gtest.h"
#include <ctime>
#include <filesystem>
#include <fstream>

void ASSERT_EQ_FILES(char const* in1, char const* in2) {
  std::ifstream fin1(in1, std::ios_base::in), fin2(in2, std::ios_base::in);
  char tmp1 = 0, tmp2 = 0;
  while (!fin1.eof() && !fin2.eof()) {
    fin1 >> tmp1;
    fin2 >> tmp2;
    ASSERT_TRUE(tmp1 == tmp2);
  }
  ASSERT_TRUE(fin1.eof() && fin2.eof());
  fin1.close();
  fin2.close();
}

const std::string path = "unit-tests";

template <bool change = true>
void htest(std::string const& input) {
#ifdef LOG
  std::cout << path << "\n";
  std::cout << input << "_Test\n";
#endif
  std::string in = (change ? (path + "/files/") : "") + input;
  std::string enc = (change ? (path + "/") : "") + input + ".huf";
  std::string dec = (change ? (path + "/") : "") + input + "_decomp";
#ifdef LOG
  time_t t1 = std::time(nullptr);
#endif
  encode(in.c_str(), enc.c_str());
#ifdef LOG
  time_t t2 = std::time(nullptr);
#endif
  decode(enc.c_str(), dec.c_str());
#ifdef LOG
  time_t t3 = std::time(nullptr);
  size_t s1 = std::filesystem::file_size(in);
  size_t s2 = std::filesystem::file_size(enc);
  size_t s3 = std::filesystem::file_size(dec);
  ASSERT_TRUE(s2 > 0);
  ASSERT_EQ(s1, s3);
  double coef = static_cast<double>(s1) / static_cast<double>(s2);
  std::cout << "Start size: " << s1
            << ", encoded_file size: " << s2
            << ", compression ratio: " << coef << "\n";
  std::cout << "Encode time: " << t2 - t1 << "s\n";
  std::cout << "Decode time: " << t3 - t2 << "s\n\n";
  ASSERT_TRUE(coef > 0.8 || s2 <= s1 + 2004);
#endif
#ifdef LOG_CLANG_PROBLEM
  cat_file(in);
  cat_file(enc);
  cat_file(dec);
#endif
  ASSERT_EQ_FILES(in.c_str(), dec.c_str());
#ifndef LEAVE_FILES
  std::filesystem::remove(enc);
  std::filesystem::remove(dec);
#endif
}

TEST(first, test) {
  htest<false>("/home/mavlyut/huffman-mavlyut/huffman-tool.cpp");
}

#define HTEST(input) TEST(correctness, input) { htest(#input); }

TEST(special, file_not_found) {
  ASSERT_THROW(encode("", ""), std::runtime_error);
}

// 0
HTEST(empty)

// 7 b
//HTEST(one_char)

// 127 b
HTEST(simple)

// 1 Kb, 165 b
HTEST(i_said_everything_i_wanted)

// 3 Kb, 167 b
HTEST(imo2022_chinese)

// 21 Kb, 667 b
HTEST(test_elf)

// 62 Kb, 839 b
HTEST(war_and_peace_wiki)

// 137 Kb, 648 b
HTEST(test_asm)

// 263 Kb, 907 b
HTEST(cpp_tutorial)

// 356 Kb, 1008 b
HTEST(some_program)

// 1 Mb, 351 Kb, 718 b
HTEST(organic_chemistry_en)

// 1 Mb, 391 Kb, 166 b
HTEST(java_tutorial)

// 5 Mb, 788 Kb, 947 b
HTEST(organic_chemistry_in_4_volumes)

#ifdef _ENABLE_BIG_TESTS

// 16 Mb, 651 Mb, 365 b
HTEST(AAA)

static char get(size_t i) {
  size_t ans = 0;
  while (i % 2) {
    i /= 2;
    ans++;
  }
  return static_cast<char>(ans + 'a');
}

// 63 Mb, 1023 Kb, 1023 b
TEST(correctness, full_abacaba) {
  std::string new_path = path + "/files/full_abacaba";
  std::ofstream fout(new_path, std::ios_base::binary | std::ios_base::out);
  size_t end = (1L << 26) - 1;
  for (size_t i = 0; i < end; i++) {
    fout << get(i);
  }
  fout.close();
  std::cout << "File generated\n";
  htest("full_abacaba");
  std::filesystem::remove(new_path);
}

// 1 Gb
TEST(correctness, bigfile) {
  srand(time(nullptr));
  std::string new_path = path + "/files/bigfile";
  std::ofstream fout(new_path, std::ios_base::binary | std::ios_base::out);
  for (size_t i = 0; i < 1024 * 1024 * 1024; i++) {
    fout << static_cast<char>(rand() % ALPHABET_SIZE + 'a');
  }
  fout.close();
  std::cout << "File generated\n";
  htest("bigfile");
  std::filesystem::remove(new_path);
}

#endif
