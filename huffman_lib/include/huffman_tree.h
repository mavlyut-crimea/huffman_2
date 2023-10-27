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

  explicit node(char_t = '$', ind_t = -1);
  node(ind_t, node const&, node const&);
  ~node();

  bool is_leaf() const;
  friend void decode(char const*, char const*);

private:
  char_t value;
  ind_t ind, left, right;
};

struct encode_huffman_tree {
  explicit encode_huffman_tree(counter const&);
  ~encode_huffman_tree();

  code_t get_code(char) const;
  friend binary_writer& write(binary_writer&, encode_huffman_tree const&);

private:
  void put_codes(ind_t, int_t, len_t);
  void write(binary_writer&, node const&) const;

  ind_t rem;
  std::vector<node> nodes;
  std::array<code_t, ALPHABET_SIZE> codes;
};

struct decode_huffman_tree {
  explicit decode_huffman_tree(binary_reader&);
  ~decode_huffman_tree();

  void move(ind_t&, bool);
  bool is_leaf(ind_t);
  char get_char(ind_t);
  ind_t get_root() const;

private:
  ind_t root;
  std::vector<node> nodes;

  ind_t read_node(std::string const&, ind_t&, ind_t, ind_t&, bool = false);
};

#endif // HUFFMAN_HUFFMAN_TREE_H
