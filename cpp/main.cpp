#include "../cpp/searchTree.h"
#include "../cpp/parser.h"

void printPath(std::vector<std::vector<std::uint32_t>> v) {
  Parser parser;
    for (int i = 0; i < v.size(); i++) {
        std::cout << "Path #" << i + 1 << ": " << std::endl;
        for (std::uint32_t num: v.at(i)) {
	  std::cout << num << ": " << parser.get_title(num) << std::endl;
        }
        std::cout << "\n";
    }
}

int main(){
	searchTree searcher;
    //timer Timer;
    //Timer.startTimer();
    //Timer.pause();
    Parser parser;
    std::unordered_map<std::string, std::uint32_t> articles = parser.get_articles_map();
    bool exit = false;
    while(true) {
      bool is_valid = false;
      std::cout << "Press ENTER with no input to exit." << std::endl;
      std::cout << "Enter the name of the starting article: ";
      std::string start_article_line;
      getline(std::cin, start_article_line);
      std::cout << std::endl;
      //parser.to_lowercase(start_article_line);
      if(start_article_line == "") {
	break;
      }
      is_valid = (articles.count(start_article_line) > 0);
      while(!is_valid) {
	std::cout << "\tInvalid article, please enter another title: ";
	getline(std::cin, start_article_line);
	//parser.to_lowercase(start_article_line);
	std::cout << std::endl;
	is_valid = (articles.count(start_article_line) > 0) && (start_article_line != "");
      }

      is_valid = false;
      std::cout << "Enter the name of the destination article: ";
      std::string end_article_line;
      getline(std::cin, end_article_line);
      std::cout << std::endl;
      //parser.to_lowercase(end_article_line);
      if(start_article_line == "") {
	break;
      }
      is_valid = articles.count(end_article_line) > 0;
      while(!is_valid) {
	std::cout << "\tInvalid article, please enter another title: ";
	getline(std::cin, end_article_line);
	//parser.to_lowercase(end_article_line);
	std::cout << std::endl;
	is_valid = articles.count(start_article_line) > 0;
      }
      std::cout << "Searching for the shortest paths... " << std::endl;
      std::uint32_t start = articles[start_article_line];
      std::uint32_t end = articles[end_article_line];
      std::cout << "start: " << std::to_string(start) << "\tend: " << std::to_string(end) << std::endl; 
      printPath(searcher.breadth_search(end, start));
      std::cout << std::endl;
    }
    //many path 0->84
    //std::cout << "SHORTEST PATH FROM " << 0 << " TO " << 24 << std::endl;
    //std::cout << "depth" << std::endl;
    //parser.parse_pageid_map();
    //parser.map_ids_memory();

    //parser.make_file_smaller();
    //Timer.unpause();
    std::uint32_t start = 9228;
    std::uint32_t end = 2905;
    //parser.parse_pagelinks();
    //std::cout << parser.get_title(45883) << std::endl;
    //std::cout << parser.get_title(45912) << std::endl;
    //std::cout << parser.get_title(45927) << std::endl;
    //std::cout << parser.pageid_to_linkid(start);
    //parser.get_parents(10866);
    //std::cout << parser.get_title(82581349);
    //parser.make_file_smaller();
    //printPath(searcher.breadth_search(end, start));
    std::uint32_t s = 2835;
    std::uint32_t t = 1238;
    //printPath(searcher.breadth_search(t, s));
    //parser.get_parents(10866);
    //std::cout << "TEST TITLE " << parser.get_title(18624) << std::endl;
    //std::cout << "breadth" << std::endl;
    //printPath(searcher.reverse_list(searcher.breadth_search(2, )));
    //Timer.printTime();
    //Timer.pause();

    //long path 3->96
    //std::cout << "SHORTEST PATH FROM " << 3 << " TO " << 96 << std::endl;
    //std::cout << "depth" << std::endl;

    //Timer.unpause();
    //printPath(searcher.depth_search(3, 96));
    //std::cout << "breadth" << std::endl;
    //printPath(searcher.reverse_list(searcher.breadth_search(3, 96)));
    //Timer.printTime();
    //Timer.pause();

    //shallow 10->73
    //std::cout << "SHORTEST PATH FROM " << 10 << " TO " << 73 << std::endl;
    //std::cout << "depth" << std::endl;
    //printPath(searcher.depth_search(10, 73));
    //std::cout << "breadth" << std::endl;
    //printPath(searcher.reverse_list(searcher.breadth_search(10, 73)));

    //deep 15 41
    //std::cout << "SHORTEST PATH FROM " << 15 << " TO " << 41 << std::endl;
    //std::cout << "depth" << std::endl;
    //printPath(searcher.depth_search(15, 41));
    //std::cout << "breadth" << std::endl;
    //printPath(searcher.reverse_list(searcher.breadth_search(15, 41)));


    return 0;
};
