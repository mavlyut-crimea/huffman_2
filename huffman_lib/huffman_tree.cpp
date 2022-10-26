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

node::node(char_t c, ind_t i): value(c), ind(i), left(-1), right(-1)  {}

node::node(ind_t i, node const& l, node const& r)
    : value(l.value + r.value), ind(i), left(l.ind), right(r.ind) {}

node::~node() = default;

bool node::is_leaf() const {
  return left == -1 && right == -1;
}

encode_huffman_tree::encode_huffman_tree(counter const& cntr)
    : char_to_ind(ALPHABET_SIZE + 1, -1) {
  std::priority_queue<std::pair<weight_t, ind_t>
                      , std::vector<std::pair<weight_t, ind_t>>
                      , std::greater<>> q;
  for (ind_t i = 0; i < ALPHABET_SIZE; i++)
    std::cout << cntr[i] << " ";
  std::cout << "\n";
  for (ind_t i = 0; i < ALPHABET_SIZE; i++) {
    if (cntr[i] > 0) {
      char_to_ind[i] = nodes.size();
      nodes.emplace_back(i, nodes.size());
      q.emplace(cntr[i], char_to_ind[i]);
    }
  }
  if (q.size() == 1) {
    ind_t letter = nodes[0].value + 1 - 2 * (nodes[0].value % 2);
    char_to_ind[letter] = 1;
    nodes.emplace_back(letter, 1);
    q.emplace(1, 1);
  }
  while (q.size() > 1) {
    auto a = q.top(); q.pop();
    auto b = q.top(); q.pop();
    nodes.emplace_back(nodes.size(), nodes[a.second], nodes[b.second]);
    q.push(std::pair(a.first + b.first, nodes.size() - 1));
  }
  codes = std::vector<code_t>(nodes.size());
  put_codes(nodes.size() - 1, 0, 0);
  ind_t rem = 0;
  for (ind_t i = 0; i < ALPHABET_SIZE; i++) {
    if (cntr[i] != 0) {
      rem = (rem + (cntr[i] % BYTESIZE) * (codes[char_to_ind[i]].second % BYTESIZE)) % BYTESIZE;
    }
  }
  char_to_ind[ALPHABET_SIZE] = rem;
}

encode_huffman_tree::~encode_huffman_tree() {
  char_to_ind.clear();
  nodes.clear();
  codes.clear();
}

// TODO: print optimized
binary_writer& write(binary_writer& bw, encode_huffman_tree const& tr) {
  ind_t tmp_ind;
  for (ind_t i = 0; i < ALPHABET_SIZE; i++) {
    tmp_ind = tr.char_to_ind[i];
    if (tmp_ind == -1)
      bw.write(0ul);
    else
      bw.write(tr.codes[tmp_ind].second).write(tr.codes[tmp_ind].first);
  }
  tmp_ind = tr.char_to_ind.back();
  if (tmp_ind == 0 && !tr.nodes.empty())
    tmp_ind = BYTESIZE;
  bw.write(tmp_ind);
  return bw;
}

static constexpr code_t EMPTY_CODE = { 0, 0 };

code_t encode_huffman_tree::get_code(char x) const {
  ind_t i = char_to_ind[to_char_t(x)];
  if (i == -1)
    return EMPTY_CODE;
  return codes[i];
}

void encode_huffman_tree::put_codes(ind_t i, int_t ct, len_t l) {
  if (i == -1)
    return;
  if (nodes[i].is_leaf()) {
    codes[char_to_ind[nodes[i].value]] = std::pair(ct, l);
    return;
  }
  put_codes(nodes[i].left, (ct << 1), l + 1);
  put_codes(nodes[i].right, (ct << 1) + 1, l + 1);
}

decode_huffman_tree::decode_huffman_tree(binary_reader& br) {
  nodes.emplace_back('$', 0); // fake root
  for (ind_t i = 0; i < ALPHABET_SIZE; i++) {
    push_code(i, br.next_code());
  }
  br.set_rem();
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
