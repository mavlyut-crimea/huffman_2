//
// Created by mavlyut on 08.08.22.
//

#include "huffman_lib/include/huffman.h"
#include <iostream>
#include <map>
#include <set>
#include <utility>

using flags_container = std::map<char, std::string>;

struct parser_exception : std::runtime_error {
  explicit parser_exception(std::string const& m)
      : std::runtime_error(
            "huffman-tool: " + m + "\nUsage: huffman-tool " +
            "(-c [COMPRESS_MODE] | -d) --input FILE_IN --output FILE_OUT" +
            "\nTry `huffman-tool --help` for more information.\n") {}
};

struct unsupported_option_exception : parser_exception {
  explicit unsupported_option_exception(std::string const& option)
      : parser_exception("unsupported option \'" + option + "\'") {}
};

struct expected_option_exception : parser_exception {
  explicit expected_option_exception(std::string const& option)
      : parser_exception("option --" + option + " requires, but not found") {}
};

struct expected_arg_exception : parser_exception {
  explicit expected_arg_exception(std::string const& option)
      : parser_exception("option --" + option + " requires argument") {}
};

struct parser_keys {
  parser_keys(flags_container flags) : flags(std::move(flags)) {}

  bool exist(char const fl) const {
    return keys.find(fl) != keys.end();
  }

  void push(char x) {
    if (x == '\0' || flags.find(x) == flags.end())
      throw unsupported_option_exception("-" + std::string(1, x));
    keys.insert(x);
    last_key = x;
  }

  void push(std::string const& k) {
    if (k.substr(0, std::min<size_t>(2, k.length())) == "--") {
      char tmp = long_to_short(flags, k.substr(2));
      if (tmp == '\0')
        throw unsupported_option_exception(k);
      return push(tmp);
    }
    if (k.substr(0, std::min<size_t>(1, k.length())) == "-") {
      for (size_t i = 1; i < k.length(); i++)
        push(k[i]);
      if (k.length() == 2)
        last_key = k[1];
      return;
    }
    if (!last_key)
      throw unsupported_option_exception(k);
    const_cast<std::string&>(keys.find(last_key)->arg) = k;
    last_key = '\0';
  }

  char const* get_arg(char fl) const {
    if (exist(fl)) {
      std::string const& ans = keys.find(fl)->arg;
      if (!ans.length())
        throw expected_arg_exception(flags.at(fl));
      return ans.data();
    }
    throw expected_option_exception(flags.at(fl));
  }

  friend std::ostream& operator<<(std::ostream& out, parser_keys const& pk) {
    for (key const& k : pk.keys)
      out << k.flag << " " << k.arg << "\n";
    return out;
  }

private:
  static char long_to_short(flags_container const& fl, std::string const& x) {
    for (const auto& it : fl)
      if (it.second == x)
        return it.first;
    return '\0';
  }

  struct key {
    key(char x) : flag(x) {}

    key(flags_container const& fl, std::string const& name)
        : flag(long_to_short(fl, name)) {}

    char flag;
    std::string arg;
  };

  struct cmp {
    bool operator()(key const& x, key const& y) const {
      return x.flag < y.flag;
    }
  };

  std::set<key, cmp> keys;
  char last_key = '\0';
  flags_container flags;
};

static const std::map<char, std::string> flags = {
    { 'h', "help" },
    { 'c', "compress" },
    { 'd', "decompress" },
    { 'i', "input" },
    { 'o', "output" }
};

int main(int argc, char** argv) {
  parser_keys pk(flags);
  for (int i = 1; i < argc; i++) {
    pk.push(std::string(argv[i]));
  }
  if (pk.exist('h') || argc == 1) {
    std::cout << "Usage: huffman-tool (-c | -d) "
              << "-i FILE_IN -o FILE_OUT\n"
              << "FLAGS:\n"
              << "\t-h, --help                more information\n"
              << "\t-c, --compress            encode file\n"
              << "\t-d, --decompress          decode file\n"
              << "\t-i, --input FILE_IN       input file\n"
              << "\t-o, --output FILE_OUT     output file\n"
              << "\nMore information: https://github.com/mavlyut-crimea/huffman-mavlyut.\n";
    return 0;
  }
  char const* in = pk.get_arg('i');
  char const* out = pk.get_arg('o');
  if (pk.exist('c') && pk.exist('d'))
    throw unsupported_option_exception("-cd");
  if (pk.exist('c'))
    encode(in, out);
  else if (pk.exist('d'))
    decode(in, out);
}
