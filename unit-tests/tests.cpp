#include <ctime>
#include <cmath>
#include <filesystem>

#include "gtest/gtest.h"
#include "../huffman_lib/huffman.h"

using namespace huffman_code_type_examples;

MODES get_mode_from_file(char const* name) {
  std::ifstream fin(name, std::ios_base::binary | std::ios_base::in);
  int mode;
  fin >> mode;
  fin.close();
  MODES ans = to_mode(mode);
  if (ans == MODES::USED) return MODES{mode};
  return ans;
}

std::ostream& operator<<(std::ostream& out, MODES x) {
  return out << static_cast<int>(x);
}

void ASSERT_EQ_FILES(char const* in1, char const* in2) {
  std::ifstream fin1(in1, std::ios_base::binary | std::ios_base::in), fin2(in2, std::ios_base::binary | std::ios_base::in);
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

void cat_file(std::string const& path) {
  std::cout << "\n__" + path + "__\n";
  char tmp;
  std::ifstream fin(path, std::ios_base::binary | std::ios_base::in);
  fin >> std::noskipws;
  while (fin >> tmp) {
    std::cout << tmp;
  }
  std::cout << "\n__end__\n";
}

const std::string path = "unit-tests";

template <typename T>
void htest(std::string const& input) {
  std::string name_of_type(typeid(T).name());
#ifdef LOG
  std::cout << path << "\n";
  std::cout << name_of_type << "_" << input << "_Test\n";
#endif
  std::string in = path + "/files/" + input;
  std::string enc = path + "/" + input + "_" + name_of_type + ".huf";
  std::string dec = path + "/" + input + "_" + name_of_type + "_decomp";
#ifdef LOG
  time_t t1 = std::time(nullptr);
#endif
//   encode<T>(in.c_str(), enc.c_str());
#ifdef LOG
  time_t t2 = std::time(nullptr);
#endif
//   decode<T>(enc.c_str(), dec.c_str());
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
            << ", encode mode: " << get_mode_from_file(enc.c_str())
            << ", compression ratio: " << coef << "\n";
  std::cout << "Encode time: " << t2 - t1 << "s\n";
  std::cout << "Decode time: " << t3 - t2 << "s\n\n";
  ASSERT_TRUE(coef > 0.8 || s2 <= s1 + 416);
#endif
// #ifdef LOG_CLANG_PROBLEM
  cat_file(in);
//   cat_file(enc);
//   cat_file(dec);
// #endif
  ASSERT_EQ_FILES(in.c_str(), dec.c_str());
  ASSERT_EQ_FILES(enc.c_str(), (path + "/files/test_elf_N26huffman_code_type_examples9ct_stringE.huf").c_str());
#ifndef LEAVE_FILES
//   std::filesystem::remove(enc);
//   std::filesystem::remove(dec);
#endif
}

#define HTEST(input)                            \
  TEST(input, string) {                         \
    htest<ct_string>(#input);                   \
  }                                             \
  TEST(input, vector_bool) {                    \
    htest<ct_vector_bool>(#input);              \
  }                                             \
  TEST(input, vector_int) {                     \
    htest<ct_vector_ints<int>>(#input);         \
  }                                             \
  TEST(input, vector_long) {                    \
    htest<ct_vector_ints<long>>(#input);        \
  }                                             \
  TEST(input, vector_long_long) {               \
    htest<ct_vector_ints<long long>>(#input);   \
  }                                             \
  TEST(input, vector_size_t) {                  \
    htest<ct_vector_ints<size_t>>(#input);      \
  }

TEST(test_elf, ct_default) {
  htest<ct_default>("test_elf");
}

// TEST(debug, simple) {
//  std::string new_path = path + "/files/full_abacaba";
//  encode(new_path.c_str(), (path + "/full_abacaba.huf").c_str());
// }

// TEST(test, read_from_br) {
//   std::fstream fstr("input", std::ios_base::out);
//   const size_t SZ = 100;
//   for (size_t i = 0; i < SZ; i++) {
//     std::cout << i << "\n";
//   }
//   fstr.open("input", std::ios_base::in);
//   buffered_reader br(fstr);
//   char tmp_char;
//   while (!br.eof()) {
//     br.read(tmp_char);
//     std::cout << tmp_char;
//   }
// }

// TEST(test, write_to_file) {
//   std::fstream fstr("input", std::ios_base::out);
//   const size_t SZ = 100;
//   for (size_t i = 0; i < SZ; i++) {
//     std::cout << i << "\n";
//   }
//   std::vector<char> buf(SZ);
//   fstr.open("input", std::ios_base::in);
//   size_t cnt = fstr.readsome(&buf[0], SZ);
//   for (size_t i = 0; i < cnt; i++) {
//     std::cout << buf[i];
//   }
// }

// TEST(special, file_not_found) {
//   ASSERT_THROW(encode("", ""), std::runtime_error);
// }

// TEST(special, empty_string) {
//   //   I still don't agree, I want to implement EFO!
//   ASSERT_NE(encode(std::string("")), "");
// }

// TEST(special, simple) {
//   std::string str("abacaba");
//   ASSERT_EQ(decode(encode(str)), str);
// }

// TEST(special, unknown_mode) {
//   ASSERT_ANY_THROW(decode("-120\nhrknf"));
// }

// TEST(special, no_mode) {
//   ASSERT_ANY_THROW(decode("hello world, i want error\n"));
// }

// TEST(special, no_mode_2) {
//   ASSERT_ANY_THROW(decode("текст на русском\n"));
// }

// TEST(special, broken_all_mode) {
//   ASSERT_ANY_THROW(decode("-1\n"));
// }

// // 0
// HTEST(empty)

// // 1 Kb, 165 b
// HTEST(i_said_everything_i_wanted)

// // 3 Kb, 167 b
// HTEST(imo2022_chinese)

// // 21 Kb, 667 b
// HTEST(test_elf)

// // 62 Kb, 839 b
// HTEST(war_and_peace_wiki)

// // 137 Kb, 648 b
// HTEST(test_asm)

// // 263 Kb, 907 b
// HTEST(cpp_tutorial)

// // 356 Kb, 1008 b
// HTEST(some_program)

// // 1 Mb, 351 Kb, 718 b
// HTEST(organic_chemistry_en)

// // 1 Mb, 391 Kb, 166 b
// HTEST(java_tutorial)

// // 5 Mb, 788 Kb, 947 b
// HTEST(organic_chemistry_in_4_volumes)

// #ifdef _ENABLE_BIG_TESTS

// // 16 Mb, 651 Mb, 365 b
// HTEST(AAA)

// static constexpr size_t ALPHABET_SIZE = 26;

// static char get(size_t i) {
//   size_t ans = 0;
//   while (i % 2) {
//     i /= 2;
//     ans++;
//   }
//   return static_cast<char>(ans + 'a');
// }

// // 63 Mb, 1023 Kb, 1023 b
// TEST(full_abacaba, ct_default) {
//   std::string new_path = path + "/files/full_abacaba";
//   std::ofstream fout(new_path, std::ios_base::binary | std::ios_base::out);
//   size_t end = (1L << ALPHABET_SIZE) - 1;
//   for (size_t i = 0; i < end; i++) {
//     fout << get(i);
//   }
//   htest<ct_default>("full_abacaba");
//   std::filesystem::remove(new_path);
// }

// // 1 Gb
// TEST(bigfile, ct_default) {
//   srand(time(nullptr));
//   std::string new_path = path + "/files/bigfile";
//   std::ofstream fout(new_path, std::ios_base::binary | std::ios_base::out);
//   for (size_t i = 0; i < 1024 * 1024 * 1024; i++) {
//     fout << static_cast<char>(rand() % ALPHABET_SIZE + 'a');
//   }
//   fout.close();
//   htest<ct_default>("bigfile");
//   std::filesystem::remove(new_path);
// }

// #endif
