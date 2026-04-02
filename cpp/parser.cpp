#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <unordered_map>
#include <cstdint>
#include "../cpp/parser.h"

// removes underscores into spaces and formats special characters
void Parser::process_title(std::string &title) {
  for(std::size_t i = 0; i < title.size(); ++i) {
    if(title[i] == '_') {
      title[i] = ' ';
    }
    else if (title[i] == '\\') {
      if(i + 1 >= title.size()) {
	  continue;
      }

      if(title[i + 1] == '\'' || title[i + 1] == '\"' || title[i + 1] == '\\' ) {
	title.erase(i, 1);
      }
    }
  }
}

// returns the index where the title field ends
std::size_t Parser::find_quote_end(const std::string& line, size_t start) {
  size_t quote_start = line.find('\'', start);
  if(quote_start != std::string::npos) {
    size_t quote_next = line.find('\'', quote_start + 1);
    bool in_quotes = true;
    while(in_quotes) {
	//std::cout << "SUBSTR: " << line.substr(quote_start, line.size() - quote_next - quote_start) << std::endl;
	std::size_t i = quote_next;
	std::size_t backslash_count = 0;
	while(i > quote_start && line[i - 1] == '\\') {
	  backslash_count++;
	  i--;
	}
	if(backslash_count % 2 == 0) {
	  in_quotes = false;
	}
	else {
	quote_next = line.find('\'', quote_next + 1);
      }
    }
    return quote_next; 
  }
  
  return std::string::npos;
}

void Parser::parse_pages() {
  // CHANGE TO NAME OF page.sql FILE
  std::ifstream input_file("../db/enwiki-20260301-page.sql");
  std::ofstream articles_file("../db/articles.bin", std::ios::binary);
  std::ofstream offsets_file("../db/article_offsets.bin", std::ios::binary);
  if(!input_file) {
    std::cout << "ERROR:\tunable to access page.sql file" << std::endl;
    return;
  }
  else if(!articles_file) {
    std::cout << "ERROR:\tunable to access articles.bin file" << std::endl;
    return;
  }
  else if(!offsets_file) {
    std::cout << "ERROR:\tunable to access article_offsets.bin" << std::endl;
    return;
  }
  std::string line;

  do {
    std::getline(input_file, line);
  }
  while(line.find("INSERT") == std::string::npos);
  
  std::uint32_t real_position = 0;
  std::uint32_t offset_position = 0;
  do { 
    std::size_t pos = 0;
    
    while((pos = line.find('(', pos)) != std::string::npos) {
      std::size_t quote_end = find_quote_end(line, pos);
      if(quote_end == std::string::npos) {break;}
      std::size_t end = line.find(')', quote_end);
      if(end == std::string::npos) {break;}

      std::string tuple = line.substr(pos + 1, end - pos - 1);
      
      std::size_t c1 = tuple.find(',');
      std::uint32_t id = static_cast<std::uint32_t>(stoi(tuple.substr(0, c1)));
      
      std::size_t c2 = tuple.find(',', c1 + 1);
      std::uint32_t name_space = static_cast<std::uint32_t>(stoi(tuple.substr(c1 + 1, c2 - c1 - 1)));
      
      // ignore etries that are not part of the main wikipedia
      if(name_space != 0) {
	pos = end + 1;
	continue;
      }
      
      std::size_t quote_start = tuple.find('\'', c1);
      quote_end = find_quote_end(tuple, 0);
      std::string title = tuple.substr(quote_start + 1, quote_end - quote_start - 1);
      process_title(title);

      std::cout << "ID: " << std::to_string(id) << std::endl;
      std::cout << "Title: " << title << "\n" << std::endl;

      // fill empty spaces in offsets file
      std::uint8_t length = title.size();
      while(real_position <= id) {
	offsets_file.write(reinterpret_cast<const char*>(&offset_position), sizeof(offset_position));
	offsets_file.write(reinterpret_cast<const char*>(&length), sizeof(length));
	real_position++;
      }
      articles_file.write(title.data(), length);
      offset_position += length;
      pos = end + 1;
    }
  }
  while(getline(input_file, line));
}

// internal test method
std::string Parser::get_title(std::uint32_t id) {
  std::ifstream articles_file("../db/articles.bin", std::ios::binary);
  std::ifstream offsets_file("../db/article_offsets.bin", std::ios::binary);

  if(!(articles_file && offsets_file)) {
    std::cout << "ERROR opening files" << std::endl;
    return "";
  }
  unsigned int offset = 0;
  uint8_t length = 0;
  offsets_file.seekg(id * (sizeof(offset) + sizeof(length)), std::ios::beg);
  offsets_file.read(reinterpret_cast<char*>(&offset), sizeof(id));
  offsets_file.read(reinterpret_cast<char*>(&length), sizeof(length));
  
  articles_file.seekg(offset, std::ios::beg);
  std::string title(length, '\0');
  articles_file.read(&title[0], length);
  return title;
}

std::uint32_t Parser::get_id(const std::string& title) {
  std::ifstream articles_file("../db/articles.bin", std::ios::binary);
  std::ifstream offsets_file("../db/article_offsets.bin", std::ios::binary);
  if(!(articles_file && offsets_file)) {
    std::cout << "ERROR opening files" << std::endl;
    return 0;
  }

  return 0;
  /*
  std::uint32_t offset = 0;
  uint8_t length = 0;
  std::uint32_t id = 0;
  std::string title(l
  while(articles.read(&title[0], length);
  */
}


std::vector<std::uint32_t> Parser::get_parents(std::uint32_t link_id) {
  std::ifstream links_file("../db/links.bin", std::ios::binary);
  std::ifstream offsets_file("../db/link_offsets.bin", std::ios::binary);

  std::vector<std::uint32_t> links;
  
  if(!(links_file && offsets_file)) {
    std::cout << "ERROR opening files" << std::endl;
    return links;
  }
  
  std::uint64_t offset = 0;
  std::uint32_t length = 0;
  offsets_file.seekg((link_id) * (sizeof(offset) + sizeof(length)));
  offsets_file.read(reinterpret_cast<char*>(&offset), sizeof(offset));
  offsets_file.read(reinterpret_cast<char*>(&length), sizeof(length));
  links_file.seekg(offset, std::ios::beg);
  
  //std::cout << "Length: " << length << std::endl;
  for(std::size_t i = 0; i < length; ++i) {
    std::uint32_t from_id = 0;
    links_file.read(reinterpret_cast<char*>(&from_id), sizeof(from_id));

    links.push_back(from_id);
    //std::cout << "Parent: " << std::to_string(from_id) << " " << get_title(from_id) << std::endl;
  }
  //std::cout << "Got parents \n\n" << links.size() << std::endl;
  return links;
}

// local helper
std::uint32_t Parser::find_first_id(const std::string& line) {
  std::size_t end = line.find(')');
  std::size_t c1 = line.find(',');
  std::size_t c2 = line.find(',', c1 + 1);
  
  return static_cast<uint32_t>(std::stoi(line.substr(c2 + 1, end - c2)));
}

// internal test method, prints every offset
void Parser::print_binary() {
  std::ifstream links_file("../db/links.bin", std::ios::binary);
  std::ifstream offsets_file("../db/link_offsets.bin", std::ios::binary);
  std::uint32_t offset = 0;
  
  while(offsets_file.read(reinterpret_cast<char*>(&offset), sizeof(offset))) {
    std::cout << offset << std::endl;
  }
}

void Parser::parse_pagelinks() {
  std::ifstream input_file("../db/enwiki-20260301-pagelinks.sql");
  std::ofstream  links_file("../db/links.bin", std::ios::binary);
  std::ofstream offsets_file("../db/link_offsets.bin", std::ios::binary);
  if(!input_file) {
    std::cout << "ERROR:\tunable to access pagelinks.sql file" << std::endl;
    return;
  }
  else if(!links_file) {
    std::cout << "ERROR:\tunable to access links.bin file" << std::endl;
    return;
  }
  else if(!offsets_file) {
    std::cout << "ERROR:\tunable to access link_offsets.bin" << std::endl;
    return;
  }
  
  std::string line;
  do {
    std::getline(input_file, line);
  }
  while(line.find("INSERT") == std::string::npos);

  std::uint32_t real_position = 0;
  std::uint64_t offset_position = 0;
  std::uint32_t last_id = find_first_id(line);
  
  std::vector<std::uint32_t> links;
  
  do {
    std::size_t pos = 0;
    while((pos = line.find('(', pos)) != std::string::npos) {
      std::size_t end = line.find(')', pos);
      std::string tuple = line.substr(pos + 1, end - pos - 1);
     
      std::size_t c1 = tuple.find(',');
      std::uint32_t from_id = static_cast<std::uint32_t>(std::stoi(tuple.substr(0, c1)));

      std::size_t c2 = tuple.find(',', c1 + 1);
      int from_name_space = std::stoi(tuple.substr(c1 + 1, c2 - c1 - 1));

      std::uint32_t to_id = static_cast<std::uint32_t>(std::stoi(tuple.substr(c2 + 1, tuple.size() - c2)));

      if(from_name_space != 0) {
	pos = end + 1;
	continue;
      }

      if(last_id == to_id) {
	links.push_back(from_id);
      }
      else {
	std::uint32_t length = links.size();
	for(std::uint32_t page_id : links) {
	  links_file.write(reinterpret_cast<const char*>(&page_id), sizeof(page_id));
	}
	
	while(real_position <= last_id) {
	  offsets_file.write(reinterpret_cast<const char*>(&offset_position), sizeof(offset_position));
	  offsets_file.write(reinterpret_cast<const char*>(&length), sizeof(length));
	  
	  real_position++;
	}
	offset_position += length  * sizeof(std::uint32_t);
	links.clear();
	links.push_back(from_id);
	last_id = to_id;
      }
      pos = end + 1;
    }
  }
  
  while(std::getline(input_file, line));
  
  // dumps the remaining links
  for(std::uint32_t page_id : links) {
    links_file.write(reinterpret_cast<const char*>(&page_id), sizeof(page_id));
  }

  std::uint32_t length = links.size();
  while(real_position <= last_id) {
    offsets_file.write(reinterpret_cast<const char*>(&offset_position), sizeof(offset_position));
    offsets_file.write(reinterpret_cast<const char*>(&length), sizeof(length));
    real_position++;
  }
}

// local debuging function
void Parser::make_file_smaller() {
  std::ifstream input_file("enwiki-20260301-pagelinks.sql");
  std::ofstream  links_file("output.txt", std::ios::binary);
  std::string line;
  do {
    std::getline(input_file, line);
    links_file << line << std::endl;
  }
  while(line.find("INSERT") == std::string::npos);
}

void Parser::parse_pageid_map() {
  // CHANGE TO NAME OF page.sql FILE
  std::ifstream input_file("../db/enwiki-20260301-page.sql");
  std::ofstream map_file("../db/pageid_map.txt");
  std::string line;
  if(!input_file) {
    std::cout << "ERROR:\tunable to access page.sql file" << std::endl;
    return;
  }
  else if(!map_file) {
    std::cout << "ERROR:\tunable to access pageid_map.txt" << std::endl;
    return;
  }
  
  do {
    std::getline(input_file, line);
  }
  while(line.find("INSERT") == std::string::npos);

  do {
    std::size_t pos = 0;
    while((pos = line.find('(', pos)) != std::string::npos){
      std::size_t quote_end = find_quote_end(line, pos);
      if(quote_end == std::string::npos) {
	break;
      }
      std::size_t end = line.find(')', quote_end);
      if(end == std::string::npos) {break;}
      std::string tuple = line.substr(pos + 1, end - pos - 1);
      std::size_t c1 = tuple.find(',');
      std::uint32_t id = stoi(tuple.substr(0, c1));
      std::size_t c2 = tuple.find(',', c1 + 1);
      int name_space = stoi(tuple.substr(c1 + 1, c2 - c1 - 1));
      // ignore entries that are not part of the main wikipedia
      if(name_space != 0) {
	pos = end + 1;
	continue;
      }
      std::size_t quote_start = tuple.find('\'', c1);
      quote_end = find_quote_end(tuple, 0);
      std::string title = tuple.substr(quote_start + 1, quote_end - quote_start - 1);
      std::cout << "ID: " << std::to_string(id) << std::endl;
      std::cout << "Title: " << title << "\n" << std::endl;
            
      map_file << title << "#" << std::to_string(id) << "\n"; 
      pos = end + 1;
    }
  }
  while(getline(input_file, line)); 
}

void Parser::parse_linkid_map() {
  // CHANGE TO NAME OF page.sql FILE
  std::ifstream input_file("../db/enwiki-20260301-linktarget.sql");
  std::ofstream map_file("../db/linkid_map.txt");
  if(!input_file) {
    std::cout << "ERROR:\tunable to access linktarget.sql file" << std::endl;
    return;
  }
  else if(!map_file) {
    std::cout << "ERROR:\tunable to access linkid_map.txt" << std::endl;
    return;
  }

  
  std::string line;
  do {
    std::getline(input_file, line);
  }
  while(line.find("INSERT") == std::string::npos);
  do {
    std::size_t pos = 0;
    while((pos = line.find('(', pos)) != std::string::npos) {
      std::size_t quote_end = find_quote_end(line, pos);
      if(quote_end == std::string::npos) {
	break;
      }
      std::size_t end = line.find(')', quote_end);
      if(end == std::string::npos) {break;}
      std::string tuple = line.substr(pos + 1, end - pos - 1);
      std::size_t c1 = tuple.find(',');
      std::uint32_t id = stoi(tuple.substr(0, c1));
      std::size_t c2 = tuple.find(',', c1 + 1);
      int name_space = stoi(tuple.substr(c1 + 1, c2 - c1 - 1));
      
      // ignore entries that are not part of the main wikipedia
      if(name_space != 0) {
	pos = end + 1;
	continue;
      }
      
      std::size_t quote_start = tuple.find('\'', c1);
      quote_end = find_quote_end(tuple, 0);
      std::string title = tuple.substr(quote_start + 1, quote_end - quote_start - 1);
      std::cout << "ID: " << std::to_string(id) << std::endl;
      std::cout << "Title: " << title << "\n" << std::endl;
      //std::cout << "TUPLE: " << tuple << "\n" << std::endl;
      map_file << title << "#" << std::to_string(id) << "\n";
      pos = end + 1;
    }
  }
  while(getline(input_file, line));
}

// asumes that parse_linkid_map and parse_pageid_map have been run;
void Parser::map_ids() {
  std::ifstream pageid_file("../db/pageid_map.txt");
  std::ifstream linkid_input("../db/linkid_map.txt");
  std::ofstream linkid_output("../db/linkids.bin", std::ios::binary);

  if(!(pageid_file && linkid_input)) {
    std::cout << "ERROR:\tunable to access input files" << std::endl;;
    return;
  }
  else if(!linkid_output) {
    std::cout << "ERROR:\tunable to access linkids.bin" << std::endl;
    return;
  }
  
  std::unordered_map<std::string, std::uint32_t> linkid_map;
  std::uint32_t index = 0;
  std::string link_line;
  while(getline(linkid_input, link_line)) {
    std::uint32_t link_divider = link_line.find('#');
    std::string link_title = link_line.substr(0, link_divider);
    std::uint32_t link_id = static_cast<std::uint32_t>(stoi(link_line.substr(link_divider + 1)));
    linkid_map[link_title] = link_id;
  }

  std::string page_line;
  while(getline(pageid_file, page_line)) {
    std::uint32_t page_divider = page_line.find('#');
    std::string page_title = page_line.substr(0, page_divider);
    std::uint32_t page_id = static_cast<std::uint32_t>(stoi(page_line.substr(page_divider + 1)));
    if(linkid_map.count(page_title) > 0) {
      std::cout << "Found: \n\t" << std::to_string(page_id) << "\t" << page_title << std::endl;
      std::uint32_t link_id = linkid_map[page_title];
      while(index <= page_id) {
	linkid_output.write(reinterpret_cast<const char*>(&link_id), sizeof(link_id));
	index++;
      }
    }
  }
}

std::uint32_t Parser::pageid_to_linkid(std::uint32_t page_id) {
  std::ifstream input_file("../db/linkids.bin", std::ios::binary);
  std::uint32_t link_id = 0;
  input_file.seekg(page_id * sizeof(link_id));
  input_file.read(reinterpret_cast<char *>(&link_id), sizeof(link_id));
  return link_id;
}

std::unordered_map<std::string, std::uint32_t> Parser::get_articles_map() {
  std::ifstream pageid_file("../db/pageid_map.txt");
  if(!pageid_file) {
    std::cout << "ERROR: unable to access pageid_map.txt" << std::endl;
  }
  std::unordered_map<std::string, std::uint32_t> title_to_id;
  std::string page_line;
  while(getline(pageid_file, page_line)) {
    std::uint32_t page_divider = page_line.find('#');
    std::string page_title = page_line.substr(0, page_divider);
    process_title(page_title);
    //to_lowercase(page_title);
    std::uint32_t page_id = static_cast<std::uint32_t>(stoi(page_line.substr(page_divider + 1)));
    title_to_id[page_title] = page_id;
  }
  return title_to_id;
}

void Parser::to_lowercase(std::string& title) {
  for(std::size_t i = 0; i < title.size(); ++i) {
    title[i] = std::tolower(title[i]);
  }
}
