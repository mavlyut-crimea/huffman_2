//
// Created by mavlyut on 18.07.22.
//

#ifndef HUFFMAN_HUFFMAN_TREE_H
#define HUFFMAN_HUFFMAN_TREE_H

#include <cstddef>
#include <vector>
#include <string>
#include <queue>
#include <cmath>

#include "huffman_code.h"

constexpr size_t MAX_SIZE = 256;
#define char_t unsigned char
#define weight_t size_t

enum MODES : int {
  ONE_CHAR = -2,
  ALL,
  USED,
  UNKNOWN = MAX_SIZE + 1
};

MODES to_mode(int x) {
  if (x < -2) return UNKNOWN;
  if (x < 0) return static_cast<MODES>(x);
  if (x <= MAX_SIZE) return USED;
  return UNKNOWN;
}

constexpr static char_t to_char_t(char x) {
  return static_cast<char_t>(static_cast<int>(x) + MAX_SIZE / 2);
}

constexpr static char from_char_t(char_t x) {
  return static_cast<char>(static_cast<int>(x) - MAX_SIZE / 2);
}

constexpr static size_t count_of_digits(size_t x) {
  if (!x) return 1;
  return static_cast<size_t>(floor(log10(static_cast<double>(x)) + 1));
}

template <typename code_t = huffman_code_type_examples::ct_default,
    typename std::enable_if<std::is_base_of<huffman_code_type, code_t>::value
        && !std::is_same<huffman_code_type, code_t>::value, void*>::type = nullptr>
struct tree {
  tree() : cnt_used(0), root(new node()),
        weights(std::vector<weight_t>(MAX_SIZE, 0)),
        codes(std::vector<code_t>(MAX_SIZE, code_t())) {}

  ~tree() {
    clear(get_root());
    weights.clear();
    codes.clear();
  }

  void inc(char_t x) {
    std::cout << static_cast<int>(x) << " ";
#ifdef LOG_CLANG_PROBLEM
    std::cout << from_char_t(x) << " " << weights[x] << " ";
#endif
    if (weights[x]++ == 0) {
      cnt_used++;
    }
#ifdef LOG_CLANG_PROBLEM
    std::cout << weights[x] << "\n";
#endif
  }

  code_t const& get_code(char_t x) const {
    return *(codes.begin() + x);
  }

  void build_tree() {
    std::priority_queue<node*, std::vector<node*>, comparator_nodes> q;
    for (weight_t i = 0; i < MAX_SIZE; i++) {
      if (weights[i]) {
        q.push(new node(i, weights[i]));
      }
    }
    while (q.size() > 1) {
      node* x = q.top();
      q.pop();
      node* y = q.top();
      q.pop();
      q.push(new node(x, y));
    }
    if (q.size()) {
      delete root.left;
      root.left = q.top();
    }
    code_t tmp_code;
    put_codes(get_root(), tmp_code);
  }

  void push(std::string const& cd, char letter) {
    if (!cd.length()) {
      return;
    }
    node* tmp = get_root();
    size_t i = 0;
    for (; i < cd.length() && !tmp->is_semi_leaf(cd[i] == '1'); i++) {
      tmp = (cd[i] == '1' ? tmp->right : tmp->left);
    }
    for (; i < cd.length() - 1; i++) {
      (cd[i] == '1' ? tmp->right : tmp->left) = new node();
      tmp = (cd[i] == '1' ? tmp->right : tmp->left);
    }
    (cd[i] == '1' ? tmp->right : tmp->left) = new node(to_char_t(letter));
  }

  friend std::basic_ostream<char>&
  operator<<(std::basic_ostream<char>& out, tree<code_t> const& x) {
    if (x.a_mode_is_better()) {
      out << MODES::ALL << '\n';
      for (code_t const& i : x.codes) {
        print(out, i);
        out << '\n';
      }
    } else {
      out << x.cnt_used << '\n';
      for (size_t i = 0; i < MAX_SIZE; i++) {
        if (x.weights[i]) {
          out << from_char_t(i);
          print(out, x.codes[i]);
        }
      }
    }
    return out;
  }

private: // structures
  struct node {
    char_t value;
    weight_t weight;
    node* left;
    node* right;

    explicit node(node* left = nullptr, node* right = nullptr)
        : value(0), weight(get_weight(left) + get_weight(right)),
          left(left), right(right) {}

    explicit node(char_t value, weight_t weight = 1)
        : value(value), weight(weight),
          left(nullptr), right(nullptr) {}

    bool is_leaf() const {
      return !left && !right;
    }

    bool is_semi_leaf(bool is_right) const {
      if (is_right) return !right;
      return !left;
    }
  };

  struct comparator_nodes {
    bool operator()(node const* a, node const* b) const {
      return get_weight(a) > get_weight(b);
    }
  };

private: // fields
  size_t cnt_used;
  node root;
  std::vector<weight_t> weights;
  std::vector<code_t> codes;

private: // methods
  void clear(node const* x) {
    if (!x) return;
    clear(x->left);
    clear(x->right);
    delete x;
  }

  void put_codes(node* tmp, code_t& tmp_code) {
    if (!tmp) {
      return;
    }
    if (tmp->is_leaf()) {
      codes[tmp->value] = tmp_code;
      return;
    }
    tmp_code.push_left_code();
    put_codes(tmp->left, tmp_code);
    tmp_code.pop_back();
    tmp_code.push_right_code();
    put_codes(tmp->right, tmp_code);
    tmp_code.pop_back();
  }

  friend weight_t get_weight(node const* x) {
    if (x) return x->weight;
    return 0;
  }

  bool a_mode_is_better() const {
    // Size_a = LEN + 3 * MAX_SIZE - cnt_used + 2
    // Size_u = LEN + MAX_SIZE + 2 * cnt_used + 1 + floor(log10(cnt_used) + 1)
    // So, a_mode is better, when Size_a < Size_u
    // TODO: recalc
    return 2 * MAX_SIZE + 1 < 3 * cnt_used + count_of_digits(cnt_used);
  }

public: // getters
  node* get_root() const {
    return root.left;
  }

  size_t get_cnt_used() const {
    return cnt_used;
  }

  weight_t get_count(char_t ch) {
    return weights[ch];
  }
};

#endif // HUFFMAN_HUFFMAN_TREE_H
