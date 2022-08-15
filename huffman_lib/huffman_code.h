//
// Created by mavlyut on 23.07.22.
//

#ifndef HUFFMAN_HUFFMAN_CODE_H
#define HUFFMAN_HUFFMAN_CODE_H

#include <ostream>
#include <vector>
#include <concepts>

#include "bstream.h"

static constexpr size_t BLOCK_SIZE = sizeof(char) * 8;

struct huffman_code_type {
  huffman_code_type() = default;
  huffman_code_type(huffman_code_type const&) = default;
  huffman_code_type& operator=(huffman_code_type const&) = default;
  virtual ~huffman_code_type() = default;
  virtual void push_left_code() = 0;
  virtual void push_right_code() = 0;
  virtual size_t size() const = 0;
  virtual bool pop_back() = 0; // has right_code been removed
  void print(std::basic_ostream<char>&) const;
  void print(obstream&) const;
  virtual bool operator[](size_t) const = 0;
};

template <typename T>
concept Is_Code_t = std::is_base_of<huffman_code_type, T>::value
    && !std::is_same<huffman_code_type, T>::value;

std::basic_ostream<char>& operator<<(std::basic_ostream<char>&, huffman_code_type const&);
obstream& operator<<(obstream&, huffman_code_type const&);

namespace huffman_code_type_examples {
  struct ct_string : huffman_code_type {
    ct_string();
    ~ct_string() override;
    void push_left_code() override;
    void push_right_code() override;
    size_t size() const override;
    bool pop_back() override;
    bool operator[](size_t) const override;

  private:
    std::string _str;
  };

  struct ct_vector_bool : huffman_code_type {
    ct_vector_bool();
    ~ct_vector_bool() override;
    void push_left_code() override;
    void push_right_code() override;
    size_t size() const override;
    bool pop_back() override;
    bool operator[](size_t) const override;

  private:
    std::vector<bool> _vec;
  };

  template <typename _int_type = uint8_t>
  requires std::is_integral<_int_type>::value
  struct ct_vector_ints : huffman_code_type {
    ct_vector_ints() : pos_end(_bitness) {}

    ~ct_vector_ints() override {
      _vec.clear();
    }

    void push_left_code() override {
      if (pos_end == _bitness) {
        _vec.push_back(0);
        pos_end = 0;
      }
      _vec.back() <<= 1;
      pos_end++;
    }

    void push_right_code() override {
      push_left_code();
      _vec.back()++;
    }

    size_t size() const override {
      if (_vec.empty()) {
        return 0;
      }
      return (_vec.size() - 1) * _bitness + pos_end;
    }

    bool pop_back() override {
      bool ans = _vec.back() & 1;
      if (!--pos_end) {
        _vec.pop_back();
        pos_end = _bitness;
      } else {
        _vec.back() >>= 1;
      }
      return ans;
    }

    bool operator[](size_t i) const override {
      size_t tmp = _vec[i / _bitness];
      return (tmp >> ((tmp == _vec.back() ? pos_end : _bitness) - 1 - i % _bitness)) & 1;
    }

  private:
    static constexpr size_t _bitness = sizeof(_int_type) * 8;
    std::vector<_int_type> _vec;
    size_t pos_end;   // always in (0, _bitness]
  };

  using ct_default = ct_string;
}

#endif // HUFFMAN_HUFFMAN_CODE_H
