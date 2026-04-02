#include <iostream>
#include <fstream>
#include <string>
#include <vector>
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
  std::ifstream input_file("enwiki-20260301-page.sql");
  std::ofstream articles_file("articles.bin", std::ios::binary);
  std::ofstream offsets_file("article_offsets.bin", std::ios::binary);
  
  std::string line;

  do {
    std::getline(input_file, line);
  }
  while(line.find("INSERT") == std::string::npos);
  
  int real_position = 0;
  unsigned int offset_position = 0;
  do { 
    std::size_t pos = 0;
    
    while((pos = line.find('(', pos)) != std::string::npos) {
      std::size_t quote_end = find_quote_end(line, pos);
      if(quote_end == std::string::npos) {break;}
      std::size_t end = line.find(')', quote_end);
      if(end == std::string::npos) {break;}

      std::string tuple = line.substr(pos + 1, end - pos - 1);
      
      std::size_t c1 = tuple.find(',');
      std::uint32_t id = stoi(tuple.substr(0, c1));
      
      std::size_t c2 = tuple.find(',', c1 + 1);
      int name_space = stoi(tuple.substr(c1 + 1, c2 - c1 - 1));
      
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
  std::ifstream articles_file("./db/articles.bin", std::ios::binary);
  std::ifstream offsets_file("./db/article_offsets.bin", std::ios::binary);

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
  std::ifstream articles_file("./db/articles.bin", std::ios::binary);
  std::ifstream offsets_file("./db/article_offsets.bin", std::ios::binary);
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
  std::ifstream links_file("./db/links.bin", std::ios::binary);
  std::ifstream offsets_file("./db/link_offsets.bin", std::ios::binary);

  std::vector<std::uint32_t> links;
  
  if(!(links_file && offsets_file)) {
    std::cout << "ERROR opening files" << std::endl;
    return links;
  }
  
  std::uint32_t offset = 0;
  std::uint32_t length = 0;
  offsets_file.seekg((link_id) * sizeof(offset));
  offsets_file.read(reinterpret_cast<char*>(&offset), sizeof(offset));
  offsets_file.read(reinterpret_cast<char*>(&length), sizeof(length));
  links_file.seekg(offset, std::ios::beg);
  
  std::cout << "Length: " << length << std::endl;
  for(std::size_t i = 0; i < length; ++i) {
    unsigned char bytes[3];
    links_file.read(reinterpret_cast<char*>(bytes), 3);

    std::uint32_t from_id = (std::uint32_t(bytes[0])) | (std::uint32_t(bytes[1]) << 8) | (std::uint32_t(bytes[2]) << 16);
    //if(from_id <= 46114) {
      links.push_back(from_id);
      std::cout << "Parent: " << std::to_string(from_id) << " " << get_title(from_id) << std::endl;
      //}
  }
  std::cout << "Got parents \n\n" << links.size() << std::endl;
  return links;
}

std::uint32_t Parser::find_first_id(const std::string& line) {
  std::size_t end = line.find(')');
  std::size_t c1 = line.find(',');
  std::size_t c2 = line.find(',', c1 + 1);
  
  return static_cast<uint32_t>(std::stoi(line.substr(c2 + 1, end - c2)));
}

// internal test method, prints every offset
void Parser::print_binary() {
  std::ifstream links_file("./db/links.bin", std::ios::binary);
  std::ifstream offsets_file("./db/link_offsets.bin", std::ios::binary);
  std::uint32_t offset = 0;
  
  while(offsets_file.read(reinterpret_cast<char*>(&offset), sizeof(offset))) {
    std::cout << offset << std::endl;
  }
}

void Parser::parse_pagelinks() {
  std::ifstream input_file("enwiki-20260301-pagelinks.sql");
  std::ofstream  links_file("./db/links.bin", std::ios::binary);
  std::ofstream offsets_file("./db/link_offsets.bin", std::ios::binary);
  
  std::string line;
  do {
    std::getline(input_file, line);
  }
  while(line.find("INSERT") == std::string::npos);

  std::uint32_t real_position = 0;
  std::uint32_t offset_position = 0;
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
 
	for(std::uint32_t id : links) {
	  std::uint32_t mask = id & 0xFFFFFF;
	  unsigned char bytes[3];
	  bytes[0] = mask & 0xFF;
	  bytes[1] = (mask >> 8) & 0xFF;
	  bytes[2] = (mask >> 16) & 0xFF;

	  links_file.write(reinterpret_cast<char*>(bytes), 3);
	  
	}
	while(real_position <= last_id) {
	  offsets_file.write(reinterpret_cast<const char*>(&offset_position), sizeof(offset_position));
	  offsets_file.write(reinterpret_cast<const char*>(&length), sizeof(length));
	  
	  real_position++;
	}
	offset_position += length  * 3;
	links.clear();
	links.push_back(from_id);
	last_id = to_id;
      }

      pos = end + 1;
	 
    }
  }
  while(std::getline(input_file, line));
  
  // dumps the remaining links
  for(std::uint32_t id : links) {
    std::uint32_t mask = id & 0xFFFFFF;
    unsigned char bytes[3];
    bytes[0] = mask & 0xFF;
    bytes[1] = (mask >> 8) & 0xFF;
    bytes[2] = (mask >> 16) & 0xFF;
    links_file.write(reinterpret_cast<char*>(bytes), 3);
  }
  std::uint32_t length = links.size();
  while(real_position <= last_id) {
    offsets_file.write(reinterpret_cast<const char*>(&offset_position), sizeof(offset_position));
    links_file.write(reinterpret_cast<const char*>(&length), sizeof(length));
    real_position++;
  }
}

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
  std::ifstream input_file("enwiki-20260301-page.sql");
  std::ofstream map_file("pageid_map.txt");
  std::string line;
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
  std::ifstream input_file("enwiki-20260301-linktarget.sql");
  std::ofstream map_file("linkid_map.txt");
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

void Parser::map_ids() {
  std::ifstream pageid_map("pageid_map.txt");
  std::ifstream linkid_map("linkid_map.txt");
  std::ofstream linkid_file("linkids.bin", std::ios::binary);
  std::string page_line;
  std::uint32_t index = 0;
  while(getline(pageid_map, page_line)) {
    std::uint32_t page_divider = page_line.find('#');
    std::string page_title = page_line.substr(0, page_divider);
    std::uint32_t page_id = static_cast<std::uint32_t>(stoi(page_line.substr(page_divider + 1)));

    std::string link_line;
    std::cout << "Searching for: " << page_title << " ..." << std::endl;
    while(getline(linkid_map, link_line)) {
	std::uint32_t link_divider = link_line.find('#');
	std::string link_title = link_line.substr(0, link_divider);
	std::uint32_t link_id = static_cast<std::uint32_t>(stoi(link_line.substr(link_divider + 1)));
	if(page_title == link_title) {
	  std::cout << "\tFOUND: " << link_title << std::endl;
	  while(index <= page_id) {
	    index++;
	    linkid_file.write(reinterpret_cast<const char*>(&link_id), sizeof(link_id));
	  }
	  //linkid_file << std::to_string(page_id) << ":" << std::to_string(link_id) << "\n";
	  break;
	}
    }
    linkid_map.clear();
    linkid_map.seekg(0, std::ios::beg);
  }
}

std::uint32_t Parser::pageid_to_linkid(std::uint32_t page_id) {
  std::ifstream input_file("linkids.bin", std::ios::binary);
  std::uint32_t link_id = 0;
  input_file.seekg(page_id * sizeof(link_id));
  input_file.read(reinterpret_cast<char *>(&link_id), sizeof(link_id));
  return link_id;
}
