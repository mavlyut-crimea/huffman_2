//
// Created by mavlyut on 08.08.22.
//

#include <iostream>
#include <set>
#include <map>
#include <utility>
#include "huffman.h"

using namespace huffman_code_type_examples;
using flags_container = std::map<char, std::string>;

struct parser_exception : std::runtime_error {
  explicit parser_exception(std::string const& m)
      : std::runtime_error(
            "huffman-tool: " + m + "\nUsage: huffman-tool " +
            "(-c [COMPRESS_MODE] | -d) [CODE_TYPE] --input FILE_IN --output FILE_OUT" +
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
    if (flags.find(x) != flags.end() || x == '\0') {
      throw unsupported_option_exception("-" + std::string(1, x));
    }
    keys.insert(x);
  }

  void push(std::string const& k) {
    if (k.substr(0, std::min<size_t>(2, k.length())) == "--") {
      char tmp = long_to_short(flags, k.substr(2));
      if (tmp == '\0') {
        throw unsupported_option_exception(k);
      }
      return push(tmp);
    }
    if (k.substr(0, std::min<size_t>(1, k.length())) == "-") {
      for (size_t i = 1; i < k.length(); i++) {
        push(k[i]);
      }
      if (k.length() == 2) {
        last_key = k[1];
      }
      return;
    }
    const_cast<std::string&>(keys.find(last_key)->arg) = k;
  }

  char const* get_arg(char fl) const {
    if (exist(fl)) {
      std::string const& ans = keys.find(fl)->arg;
      if (!ans.length()) {
        throw expected_arg_exception(flags.at(fl));
      }
      return ans.data();
    }
    throw expected_option_exception(flags.at(fl));
  }

  friend std::ostream& operator<<(std::ostream& out, parser_keys const& pk) {
    for (key const& k : pk.keys) {
      out << k.flag << " " << k.arg << "\n";
    }
    return out;
  }

private:
  static char long_to_short(flags_container const& fl, std::string const& x) {
    for (auto it = fl.begin(); it != fl.end(); it++) {
      if (it->second == x) {
        return it->first;
      }
    }
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
    { 'o', "output" },
    { 'i', "input" },
    { 'S', "ct=string" },
    { 'I', "ct=vector_int" },
    { 'B', "ct=vector_bool" },
    { 'A', "mode=all" },
    { 'U', "mode=used" },
    { 'D', "mode=default" }
};

int main(int argc, char** argv) {
  parser_keys pk(flags);
  for (int i = 1; i < argc; i++) {
    pk.push(std::string(argv[i]));
  }
  if (pk.exist('h') || argc == 1) {
    std::cout << "Usage: huffman-tool (-c [COMPRESS_MODE -S SMALL_SIZE] | -d) [CODE_TYPE] "
              << "--input FILE_IN --output FILE_OUT\n"
              << "FLAGS:\n"
              << "\t-h, --help                more information\n"
              << "\t-c, --compress            encode file\n"
              << "\t-d, --decompress          decode file\n"
              << "\t-i, --input FILE_IN       input file\n"
              << "\t-o, --output FILE_OUT     output file\n"
              << "\n\tCOMPRESS_MODE:\n"
              << "\t\t-A, --mode=all            write codes of all chars in CODE_TABLE\n"
              << "\t\t-U, --mode=used           write codes of used chars in CODE_TABLE\n"
              << "\t\t-D, --mode=default        (default compress mode) "
              << "use mode (a or u) which is more optimal\n"
              << "\n\tCODE_TYPE_DEFAULT:\n"
              << "\t\t-s, --ct=string             (default code type)\n"
              << "\t\t-I, --ct=vector_int (1-6)   (it doesn't always work correctly)\n"
              << "\t\t-b, --ct=vector_bool\n"
              << "OPTIMIZATIONS:\n"
              << "\t-S, --sfo (disable | <small_size = " << DEFAULT_SMALL_SIZE << ">)"
              << "\nMore information: https://github.com/mavlyut-crimea/huffman-mavlyut.\n"
        ;
    return 0;
  }
  char const* in = pk.get_arg('i');
  char const* out = pk.get_arg('o');
//  size_t small_size = DEFAULT_SMALL_SIZE;
//  if (pk.exist('S')) {
//    char const* arg = pk.get_arg('S');
//    if (std::string(arg) == "disable") {
//      small_size = std::numeric_limits<size_t>::max();
//    } else {
//      small_size = atoi(arg);
//    }
//  }
  if (pk.exist('c')) {
    if (pk.exist('d')) {
      throw unsupported_option_exception("-cd");
    }
    // TODO: add small size
    if (pk.exist('s')) {
      encode<ct_string>(in, out);
    } else if (pk.exist('I')) {
      int bitness = atoi(pk.get_arg('I'));
      if (bitness > 6 || bitness < 0)
      encode<ct_vector_ints<>>(in, out);
    } else if (pk.exist('b')) {
      encode<ct_vector_bool>(in, out);
    } else {
      encode<ct_default>(in, out);
    }
  } else if (pk.exist('d')) {
    if (pk.exist('c')) {
      throw unsupported_option_exception("-cd");
    }
    if (pk.exist('s')) {
      decode<ct_string>(in, out);
    } else if (pk.exist('I')) {
      decode<ct_vector_ints<int>>(in, out);
    } else if (pk.exist('b')) {
      decode<ct_vector_bool>(in, out);
    } else {
      decode<ct_default>(in, out);
    }
  }
}
