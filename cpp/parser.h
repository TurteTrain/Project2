#pragma once
#include <fstream>
#include <string>
#include <vector>
#include <cstdint>
#include <unordered_map>

class Parser {
  static void process_title(std::string &title);
  std::size_t find_quote_end(const std::string& line, size_t start);
  std::uint32_t find_first_id(const std::string& line);
  
public:
  void make_file_smaller();
  void print_binary();
  void parse_pages();
  void parse_pagelinks();
  void parse_pageid_map();
  void parse_linkid_map();
  void map_ids();
  void map_ids_memory();
  static void to_lowercase(std::string &title);
  // return the title of the page at index id
  static std::string get_title(std::uint32_t page_id);
  // return the parents of an article, given its id
  static std::vector<std::uint32_t> get_parents(unsigned int id);
  static std::uint32_t get_id(const std::string& title);
  static std::uint32_t pageid_to_linkid(std::uint32_t page_id);
  static std::unordered_map<std::string, std::uint32_t> get_articles_map();
};
