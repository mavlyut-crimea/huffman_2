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

// TODO: I haven't calculate optimal SMALL_SIZE yet, so why won't it be 416
template <typename code_t = huffman_code_type_examples::ct_default,
    size_t SMALL_SIZE = 416> requires Is_Code_t<code_t>
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
    if (weights[x]++ == 0) {
      cnt_used++;
    }
  }

  code_t const& get_code(char_t x) const {
    return *(codes.begin() + x);
  }

  void build_tree() {
    std::priority_queue<node*, std::vector<node*>, comparator_nodes> q;
    char_t last_used;
    for (weight_t i = 0; i < MAX_SIZE; i++) {
      if (weights[i]) {
        last_used = i;
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
    if (q.size()) root.left = q.top();
    code_t tmp_code;
    if (cnt_used != 1) {
      put_codes(get_root(), tmp_code);
    } else {
      tmp_code.push_left_code();
      codes[last_used] = tmp_code;
    }
  }

  void push(std::string const& cd, char letter) {
    node* tmp = get_root();
    size_t i = 0;
    for (; i < cd.length() && !is_semi_leaf(tmp, cd[i] == '1'); i++) {
      tmp = (cd[i] == '1' ? tmp->right : tmp->left);
    }
    for (; i < cd.length() - 1; i++) {
      (cd[i] == '1' ? tmp->right : tmp->left) = new node();
      tmp = (cd[i] == '1' ? tmp->right : tmp->left);
    }
    (cd[i] == '1' ? tmp->right : tmp->left) = new node(to_char_t(letter));
  }

  bool need_to_compress(size_t start_size) const {
#ifdef _SFO
    if (start_size < SMALL_SIZE) {
      return false;
    }
#endif
#ifdef _DCO
    size_t tmp_size = 0, i = 0;
    while (i < MAX_SIZE) {
      tmp_size += codes[i].size() * weights[i];
      i++;
      if (tmp_size > start_size) {
        return false;
      }
    }
#endif
    return true;
  }

  friend std::basic_ostream<char>&
  operator<<(std::basic_ostream<char>& out, tree<code_t> const& x) {
    if (x.a_mode_is_better()) {
      out << "-1\n";
      for (code_t const& i : x.codes) {
        out << i << '\n';
      }
    } else {
      out << x.cnt_used << '\n';
      for (size_t i = 0; i < MAX_SIZE; i++) {
        if (x.weights[i]) {
          out << from_char_t(i) << x.codes[i];
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

    friend bool is_leaf(node const* x) {
      if (!x) return true;
      return x->is_leaf();
    }

    bool is_semi_leaf(bool is_right) const {
      if (is_right) return !right;
      return !left;
    }

    friend bool is_semi_leaf(node const* x, bool is_right) {
      if (!x) return true;
      return x->is_semi_leaf(is_right);
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
#ifdef _MODE_A
    return true;
#else
  #ifdef _MODE_U
    return false;
  #else
    return 2 * MAX_SIZE + 1 < 3 * cnt_used + count_of_digits(cnt_used);
  #endif
#endif
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
