//
// Created by mavlyut on 23.07.22.
//

#ifndef HUFFMAN_HUFFMAN_CODE_H
#define HUFFMAN_HUFFMAN_CODE_H

#include <ostream>
#include <vector>
#include <concepts>

#include "bstream.h"

struct huffman_code_type {
  huffman_code_type() = default;
  huffman_code_type(huffman_code_type const&) = default;
  huffman_code_type& operator=(huffman_code_type const&) = default;
  virtual ~huffman_code_type() = default;
  virtual void push_left_code() = 0;
  virtual void push_right_code() = 0;
  void push_code(bool is_right);
  virtual bool is_empty() const = 0;
  virtual size_t size() const = 0;
  virtual bool pop_front() = 0; // has right_code been removed
  virtual bool pop_back() = 0; // has right_code been removed
  std::string to_string() const;
  virtual void print(std::basic_ostream<char>&) const = 0;
  virtual void print_optimized(std::basic_ostream<char>&) const = 0;
  virtual void print_optimized(obstream&) const = 0;
  virtual void reset() = 0;
};

template <typename T>
concept Is_Code_t = std::is_base_of<huffman_code_type, T>::value
    && !std::is_same<huffman_code_type, T>::value;

std::basic_ostream<char>& operator<<(std::basic_ostream<char>&, huffman_code_type const&);
obstream& operator<<(obstream&, huffman_code_type const&);

template <typename T>
requires Is_Code_t<T>
T& copy_from_string(std::string const&);

namespace huffman_code_type_examples {
  struct ct_string : huffman_code_type {
    ct_string();
    ~ct_string() override;
    void push_left_code() override;
    void push_right_code() override;
    bool is_empty() const override;
    size_t size() const override;
    bool pop_front() override;
    bool pop_back() override;
    void print(std::basic_ostream<char>&) const override;
    void print_optimized(std::basic_ostream<char>&) const override;
    void print_optimized(obstream&) const override;
    void reset() override;

  private:
    std::string _str;
  };

  struct ct_vector_bool : huffman_code_type {
    ct_vector_bool();
    ~ct_vector_bool() override;
    void push_left_code() override;
    void push_right_code() override;
    bool is_empty() const override;
    size_t size() const override;
    bool pop_front() override;
    bool pop_back() override;
    void print(std::basic_ostream<char>&) const override;
    void print_optimized(std::basic_ostream<char>&) const override;
    void print_optimized(obstream&) const override;
    void reset() override;

  private:
    std::vector<bool> _vec;
  };

  struct ct_vector_int : huffman_code_type {
    using _int_type = uint8_t;
    static constexpr size_t _bitness = sizeof(_int_type) * 8;

    ct_vector_int();
    ~ct_vector_int() override;
    void push_left_code() override;
    void push_right_code() override;
    bool is_empty() const override;
    size_t size() const override;
    bool pop_front() override;
    bool pop_back() override;
    void print(std::basic_ostream<char>&) const override;
    void print_optimized(std::basic_ostream<char>&) const override;
    void print_optimized(obstream&) const override;
    void reset() override;

  private:
    std::vector<_int_type> _vec;
    size_t pos_start; // always in [0, _bitness)
    size_t pos_end;   // always in (0, _bitness]
  };

//  using ct_default = ct_vector_bool;
  using ct_default = ct_string;
}

#endif // HUFFMAN_HUFFMAN_CODE_H
