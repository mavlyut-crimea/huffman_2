//
// Created by mavlyut on 01/10/22.
//

#ifndef HUFFMAN_HUFFMAN_TREE_H
#define HUFFMAN_HUFFMAN_TREE_H

#include "binary_reader.h"
#include "binary_writer.h"
#include "consts.h"
#include "counter.h"
#include <set>

struct node {
  friend struct encode_huffman_tree;
  friend struct decode_huffman_tree;

  node(char_t, ind_t);
  node(ind_t, node const&, node const&);
  ~node();

  bool is_leaf() const;
  friend void decode(char const*, char const*);

private:
  char_t value;
  ind_t ind, left, right;
};

struct encode_huffman_tree {
  explicit encode_huffman_tree(counter&);
  ~encode_huffman_tree();

  code_t get_code(char) const;
  friend binary_writer& write(binary_writer&, encode_huffman_tree const&
      , std::array<weight_t, ALPHABET_SIZE> const&);

private:
  void put_codes(ind_t, int_t, len_t);

  std::array<ind_t, ALPHABET_SIZE + 1> char_to_ind{};
  std::vector<node> nodes;
  std::vector<code_t> codes;
};

struct decode_huffman_tree {
  explicit decode_huffman_tree(binary_reader&);
  ~decode_huffman_tree();

  void move(ind_t&, bool);
  bool is_leaf(ind_t);
  char get_char(ind_t);

private:
  std::vector<node> nodes;

  bool is_semi_leaf(ind_t, bool);
  void push_code(char_t, len_t, int_t);
};

#endif // HUFFMAN_HUFFMAN_TREE_H
