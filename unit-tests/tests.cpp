#include "../huffman_lib/include/consts.h"
#include "../huffman_lib/include/huffman.h"
#include "gtest/gtest.h"
#include <filesystem>
#include <fstream>

void ASSERT_EQ_FILES(char const* in1, char const* in2) {
  std::ifstream fin1(in1, std::ios_base::in), fin2(in2, std::ios_base::in);
  char tmp1 = 0, tmp2 = 0;
  while (!fin1.eof() && !fin2.eof()) {
    fin1 >> tmp1;
    fin2 >> tmp2;
    ASSERT_EQ(tmp1, tmp2);
  }
  ASSERT_TRUE(fin1.eof() && fin2.eof());
  fin1.close();
  fin2.close();
}

const std::string path = "unit-tests";

void htest(std::string const& input) {
  std::string in = path + "/files/" + input;
  std::string enc = path + "/" + input + ".huf";
  std::string dec = path + "/" + input + "_decomp";
  encode(in.c_str(), enc.c_str());
  decode(enc.c_str(), dec.c_str());
  ASSERT_EQ_FILES(in.c_str(), dec.c_str());
  std::filesystem::remove(enc);
  std::filesystem::remove(dec);
}

#define HTEST(input) TEST(correctness, input) { htest(#input); }

TEST(special, file_not_found) {
  ASSERT_THROW(encode("", ""), std::runtime_error);
}

#define THROW_DECODE_TEST(input) TEST(special, input) {               \
    ASSERT_THROW(decode((path + "/files/"+ #input).c_str(),           \
                 (path + "/files/" + #input + ".decomp").c_str()),    \
                 std::runtime_error);                                 \
  }

THROW_DECODE_TEST(simple)

THROW_DECODE_TEST(bad_tree)

THROW_DECODE_TEST(no_rem)

THROW_DECODE_TEST(short_tree)

// 0
HTEST(empty)

// 7 b
HTEST(one_char)

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
  htest("bigfile");
  std::filesystem::remove(new_path);
}
