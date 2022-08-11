#include "gtest/gtest.h"
#include "../huffman_lib/huffman.h"
#include <ctime>
#include <cmath>

using namespace huffman_code_type_examples;

#define ASSERT_EQ_FILES(in1, in2) {         \
    std::ifstream fin1(in1), fin2(in2);     \
    char tmp1, tmp2;                        \
    while (!fin1.eof() && !fin2.eof()) {    \
      fin1 >> tmp1;                         \
      fin2 >> tmp2;                         \
      ASSERT_EQ(tmp1, tmp2);                \
    }                                       \
    ASSERT_TRUE(fin1.eof() && fin2.eof());  \
}

const std::string path = "/home/mavlyut/CLionProjects/huffman-mavlyut/unit-tests";
constexpr double eps = 1e-5;

#define MY_TEST(code_t, input) TEST(code_t, input) {                           \
  std::cout << test_info_->test_case_name()                                    \
              << "_" << test_info_->name() << "_Test\n";                       \
  std::string in = path + "/files/" + test_info_->name();                      \
  std::string enc = path + "/enc_files/" + test_info_->name() + "_" + #code_t; \
  std::string dec = path + "/dec_files/" + test_info_->name() + "_" + #code_t; \
  time_t t1 = std::time(nullptr);                                              \
  encode<code_t> (in.c_str(), enc.c_str());                                    \
  time_t t2 = std::time(nullptr);                                              \
  decode<code_t>(enc.c_str(), dec.c_str());                                    \
  time_t t3 = std::time(nullptr);                                              \
  size_t s1 = std::filesystem::file_size(in);                                  \
  size_t s2 = std::filesystem::file_size(enc);                                 \
  size_t s3 = std::filesystem::file_size(dec);                                 \
  ASSERT_EQ(s1, s3);                                                           \
  double coef = static_cast<double>(s1) / static_cast<double>(s2);             \
  std::cout << "Start size: " << s1 << ", encoded_file size: " << s2           \
            << ", compression ratio: " << coef << "\n";                        \
  std::cout << "Encode time: " << t2 - t1 << "s\n";                            \
  std::cout << "Decode time: " << t3 - t2 << "s\n\n";                          \
  ASSERT_EQ_FILES(in, dec);                                                    \
  /*if (_SFO || _DCO) ASSERT_LE(abs(coef - 1), eps);*/                         \
}

#define HTEST(input)              \
  MY_TEST(ct_string, input)
//  MY_TEST(ct_vector_bool, input)  \
//  MY_TEST(ct_vector_int, input)

HTEST(input)

#ifdef _ALL_TESTS

// 0
HTEST(empty)

// 7 b
HTEST(one_char)

// 127 b
HTEST(simple)

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

// 1 Mb, 351 Kb, 718 b
HTEST(organic_chemistry_en)

// 1 Mb, 391 Kb, 166 b
HTEST(java_tutorial)

// 5 Mb, 788 Kb, 947 b
//HTEST(organic_chemistry_in_4_volumes)

// 16 Mb, 651 Mb, 365 b
HTEST(AAA)

// 63 Mb, 1023 Kb, 1023 b
HTEST(full_abacaba)

// 963 Mb, 216 Kb, 128 b
//HTEST(bigfile)

#endif
