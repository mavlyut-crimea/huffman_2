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
  for (ind_t i = 0; i < ALPHABET_SIZE; i++)
    if (cntr[i] > 0) {
      nodes.emplace_back(i, nodes.size());
      q.emplace(cntr[i], nodes.size() - 1);
    }
  while (q.size() > 1) {
    auto a = q.top(); q.pop();
    auto b = q.top(); q.pop();
    nodes.emplace_back(nodes.size(), nodes[b.second], nodes[a.second]);
    q.emplace(a.first + b.first, nodes.size() - 1);
  }
  codes.fill({ 0, 0 });
  put_codes(nodes.size() - 1, 0, 0);
  for (ind_t i = 0; i < ALPHABET_SIZE; i++)
    if (cntr[i] != 0)
      rem = (rem + (cntr.get_truth_cnt(i) % BYTESIZE)
                   * (codes[i].second % BYTESIZE)) % BYTESIZE;
  if (cntr.used > 1 && rem == 0)
    rem = BYTESIZE;
}

encode_huffman_tree::~encode_huffman_tree() {
  nodes.clear();
}

binary_writer& write(binary_writer& bw, encode_huffman_tree const& tr) {
  bw.write(tr.nodes.size());
  if (!tr.nodes.empty())
    tr.write(bw, tr.nodes.back());
  bw.write(tr.rem);
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

void encode_huffman_tree::write(binary_writer& bw, node const& i) const {
  if (i.is_leaf())
    return bw.write(from_char_t(i.value));
  char x = 2 * (nodes[i.left].is_leaf()) + (nodes[i.right].is_leaf()) + '0';
  bw.write(x);
  write(bw, nodes[i.left]);
  write(bw, nodes[i.right]);
}

decode_huffman_tree::decode_huffman_tree(binary_reader& br) {
  len_t len = br.next_int();
  nodes.resize(len);
  std::string tree(len, 0);
  br.read(tree);
  ind_t last_used = 0;
  ind_t pos = 0;
  root = read_node(tree, pos, 0, last_used);
  br.set_rem();
}

decode_huffman_tree::~decode_huffman_tree() {
  nodes.clear();
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

ind_t decode_huffman_tree::get_root() const {
  return root;
}

ind_t decode_huffman_tree::read_node(std::string const& tr, ind_t& pos
    , ind_t num, ind_t& last_used, bool is_leaf) {
  char c = tr[pos++];
  if (is_leaf) {
    nodes[num] = node(to_char_t(c), num);
    last_used = num;
    return num;
  }
  if ('0' > c || c > '4')
    throw std::runtime_error("Unknown mode");
  ind_t l = read_node(tr, pos, num + 1, last_used, c == '3' || c == '2');
  ind_t r = read_node(tr, pos, last_used + 1, last_used, c == '3' || c == '1');
  nodes[num] = node(num, nodes[l], nodes[r]);
  return num;
}
