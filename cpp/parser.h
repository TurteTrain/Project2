#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <cstdint>

class Parser {
  void process_title(std::string &title);
  std::size_t find_quote_end(const std::string& line, size_t start);
  std::uint32_t find_first_id(const std::string& line);
public:
  void make_file_smaller();
  void print_binary();
  void parse_pages();
  void parse_pagelinks();
  // return the title of the page at index id
  static std::string get_title(std::uint32_t id);
  // return the parents of an article, given its id
  static std::vector<std::uint32_t> get_parents(unsigned int id);
  static std::uint32_t get_id(const std::string& title);
  
};
