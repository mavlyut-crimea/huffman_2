//
// Created by mavlyut on 01/10/22.
//

#include "include/huffman_tree.h"
#include <queue>

char_t to_char_t(char x) {
  return static_cast<char_t>(x + ALPHABET_SIZE / 2);
}

char from_char_t(char_t x) {
  return static_cast<char>(static_cast<int>(x) - ALPHABET_SIZE / 2);
}

node::node(char_t c, ind_t i) : value(c), ind(i), left(-1), right(-1)  {}

node::node(ind_t i, node const& l, node const& r)
    : value(l.value + r.value), ind(i), left(l.ind), right(r.ind) {}

node::~node() = default;

bool node::is_leaf() const {
  return left == -1 && right == -1;
}

encode_huffman_tree::encode_huffman_tree(counter const& cntr) : rem(0) {
  std::priority_queue<std::pair<weight_t, ind_t>
                      , std::vector<std::pair<weight_t, ind_t>>
                      , std::greater<>> q;
  for (ind_t i = 0; i < ALPHABET_SIZE; i++) {
    if (cntr[i] > 0) {
      nodes.emplace_back(i, nodes.size());
      q.emplace(cntr[i], nodes.size() - 1);
    }
  }
  if (q.size() == 1) {
    ind_t letter = nodes[0].value + 1 - 2 * (nodes[0].value % 2);
    nodes.emplace_back(letter, 1);
    q.emplace(1, 1);
    rem = 1;
  }
  while (q.size() > 1) {
    auto b = q.top(); q.pop();
    auto a = q.top(); q.pop();
    nodes.emplace_back(nodes.size(), nodes[a.second], nodes[b.second]);
    q.push(std::pair(a.first + b.first, nodes.size() - 1));
  }
  codes.fill({ 0, 0 });
  put_codes(nodes.size() - 1, 0, 0);
  for (ind_t i = 0; i < ALPHABET_SIZE; i++) {
    if (cntr[i] != 0) {
      rem = (rem + ((1 + cntr[i]) % BYTESIZE) * (codes[i].second % BYTESIZE)) % BYTESIZE;
    }
  }
}

encode_huffman_tree::~encode_huffman_tree() {
  nodes.clear();
}

// TODO: print optimized
binary_writer& write(binary_writer& bw, encode_huffman_tree const& tr) {
  for (ind_t i = 0; i < ALPHABET_SIZE; i++)
    bw.write(tr.codes[i].second);
  ind_t tmp_ind = tr.rem;
  if (tmp_ind == 0 && !tr.nodes.empty())
    tmp_ind = BYTESIZE;
  bw.write(tmp_ind);
  for (ind_t i = 0; i < ALPHABET_SIZE; i++)
    bw.write(tr.codes[i]);
  return bw;
}

code_t encode_huffman_tree::get_code(char x) const {
  return codes[to_char_t(x)];
}

void encode_huffman_tree::put_codes(ind_t i, int_t ct, len_t l) {
  if (i == -1)
    return;
  if (nodes[i].is_leaf()) {
    codes[nodes[i].value].first = ct;
    codes[nodes[i].value].second = l;
    return;
  }
  put_codes(nodes[i].left, (ct << 1), l + 1);
  put_codes(nodes[i].right, (ct << 1) + 1, l + 1);
}

decode_huffman_tree::decode_huffman_tree(binary_reader& br) {
  nodes.emplace_back('$', 0); // fake root
  std::array<len_t, ALPHABET_SIZE> lens{};
  lens.fill(0);
  for (ind_t i = 0; i < ALPHABET_SIZE; i++)
    br.next_len(lens[i]);
  br.set_rem();
  for (ind_t i = 0; i < ALPHABET_SIZE; i++)
    push_code(i, br.next_code(lens[i]));
}

decode_huffman_tree::~decode_huffman_tree() {
  nodes.clear();
}

void decode_huffman_tree::push_code(char_t c, code_t const& p) {
  if (p.second == 0)
    return;
  ind_t tmp_node = 0;
  len_t used_bit = 0;
  bool goto_right = (p.first >> (p.second - used_bit - 1)) & 1;
  while (used_bit < p.second && !is_semi_leaf(tmp_node, goto_right)) {
    used_bit++;
    move(tmp_node, goto_right);
    goto_right = (p.first >> (p.second - used_bit - 1)) & 1;
  }
  while (used_bit < p.second) {
    used_bit++;
    (goto_right ? nodes[tmp_node].right : nodes[tmp_node].left) = nodes.size();
    nodes.emplace_back(nodes.size(), nodes.size());
    tmp_node = nodes.size() - 1;
    goto_right = (p.first >> (p.second - used_bit - 1)) & 1;
  }
  nodes.back().value = c;
}

void decode_huffman_tree::move(ind_t& tmp_node, bool go_to_right) {
  tmp_node = go_to_right
               ? nodes[tmp_node].right
               : nodes[tmp_node].left;
}

bool decode_huffman_tree::is_leaf(ind_t tmp_node) {
  return nodes[tmp_node].is_leaf();
}

char decode_huffman_tree::get_char(ind_t tmp_node) {
  return from_char_t(nodes[tmp_node].value);
}

bool decode_huffman_tree::is_semi_leaf(ind_t tmp_node, bool right) {
  if (tmp_node >= nodes.size())
    return true;
  node& nd = nodes[tmp_node];
  return (right ? nd.right : nd.left) == -1;
}
