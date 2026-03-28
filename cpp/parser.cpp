#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cstdint>

void process_title(std::string &title) {
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
std::size_t find_quote_end(const std::string& line, size_t start) {
  size_t quote_start = line.find('\'', start);
  if(quote_start != std::string::npos) {
    size_t quote_next = line.find('\'', quote_start + 1);
    bool in_quotes = true;
    while(in_quotes) {
      if(line.at(quote_next - 1) != '\\') {
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

void parse_pages() {
  // CHANGE TO NAME OF page.sql FILE
  std::ifstream file("enwiki-20260301-page.sql");
  std::ofstream articles_file("articles.bin", std::ios::binary);
  std::ofstream offsets_file("article_offsets.bin", std::ios::binary);
  
  std::string line;

  do {
    std::getline(file, line);
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
  while(getline(file, line));
}

// internal test method
void read_pages_binary(unsigned int id) {
  std::ifstream articles_file("articles.bin", std::ios::binary);
  std::ifstream offsets_file("article_offsets.bin", std::ios::binary);

  unsigned int offset = 0;
  uint8_t length = 0;
  offsets_file.seekg(id * (sizeof(offset) + sizeof(length)), std::ios::beg);
  offsets_file.read(reinterpret_cast<char*>(&offset), sizeof(id));
  offsets_file.read(reinterpret_cast<char*>(&length), sizeof(length));

  std::cout << "offset: " << offset << std::endl;
  std::cout << "length: " << std::to_string(length) << std::endl;
  
  articles_file.seekg(offset, std::ios::beg);
  std::string title(length, '\0');
  articles_file.read(&title[0], length);
  std::cout << title << std::endl;
}

// internal test method
void read_links_binary(std::uint32_t id) {
  std::ifstream links_file("links.bin", std::ios::binary);
  std::ifstream offsets_file("link_offsets.bin", std::ios::binary);

  std::uint32_t offset = 0;
  std::uint32_t next_offset = 0;
  offsets_file.seekg(id * sizeof(offset));
  offsets_file.read(reinterpret_cast<char*>(&offset), sizeof(offset));
  offsets_file.read(reinterpret_cast<char*>(&next_offset), sizeof(next_offset));
  
  links_file.seekg(offset, std::ios::beg);
  std::size_t length = (next_offset - offset) / 3;
  
  std::vector<std::uint32_t> links(length);
  std::cout << "length_bytes " << links.size() << std::endl;

  for(std::size_t i = 0; i < length; ++i) {
    unsigned char bytes[3];
    links_file.read(reinterpret_cast<char*>(bytes), 3);

    std::uint32_t from_id = (std::uint32_t(bytes[0])) | (std::uint32_t(bytes[1]) << 8) | (std::uint32_t(bytes[2]) << 16);
    links.push_back(from_id);
  }

  std::cout << "ID: " << std::to_string(id) << std::endl;
  std::cout << "Linked from ";
    for(auto l : links) {
    std::cout << l << ", ";
  }
  std::cout << "\n\n" << std::endl;
}

std::uint32_t find_first_id(const std::string& line) {
  std::size_t end = line.find(')');
  std::size_t c1 = line.find(',');
  std::size_t c2 = line.find(',', c1 + 1);
  
  return static_cast<uint32_t>(std::stoi(line.substr(c2 + 1, end - c2)));
}

void parse_pagelinks() {
  std::ifstream file("enwiki-20260301-pagelinks.sql");
  std::ofstream  links_file("links.bin", std::ios::binary);
  std::ofstream offsets_file("link_offsets.bin", std::ios::binary);
  
  std::string line;
  do {
    std::getline(file, line);
  }
  while(line.find("INSERT") == std::string::npos);

  std::uint32_t real_position = 0;
  std::uint32_t offset_position = 0;
  size_t last_id = find_first_id(line);
  std::cout << std::to_string(last_id);
  
  
  while(getline(file, line)) {
    std::vector<std::uint32_t> links;
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
	  real_position++;
	}
	offset_position += links.size() * 3;
	links.clear();
	links.push_back(from_id);
      }

      last_id = to_id;
      pos = end + 1;
	 
    }
    // dumps the remaining links
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
      real_position++;
    }
  }
}
 
int main() {
  //parse_pages();
  parse_pagelinks();
  read_links_binary(2);
  return 0;
}
