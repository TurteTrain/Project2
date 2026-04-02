#include "../cpp/searchTree.h"
#include "../cpp/parser.h"
#include "../cpp/timer.h"

bool printPath(std::vector<std::vector<std::uint32_t>> v) {
  if(v.empty()){
    return false;
  }
  Parser parser;
    for (int i = 0; i < v.size(); i++) {
        std::cout << "Path #" << i + 1 << ": " << std::endl;
        for (std::uint32_t num: v.at(i)) {
	  std::cout << num << ": " << parser.get_title(num) << std::endl;
        }
        std::cout << "\n";
    }
    return true;
}

int main(){
    std::cout << "Loading files..." << std::endl;
	searchTree searcher;
    timer Timer;
    Timer.startTimer();
    Timer.pause();
    Parser parser;
    std::unordered_map<std::string, std::uint32_t> articles = parser.get_articles_map();
    bool exit = false;

    //MAIN LOOP-----------------------------------------------------------
    while(true) {
      //FIRST INPUT-----------------------------------------------------
      bool is_valid = false;
      std::cout << "Press ENTER with no input to exit." << std::endl;
      std::cout << "Enter the exact name of the starting article: ";
      std::string start_article_line;
      getline(std::cin, start_article_line);
      std::cout << std::endl;
      if(start_article_line == "") {
	      return 0;
      }
      is_valid = (articles.count(start_article_line) > 0);
      while(!is_valid) {
        std::cout << "Invalid article, please enter another title: ";
	    getline(std::cin, start_article_line);
	    std::cout << std::endl;
	    if(start_article_line == "") {
	      return 0;
        }
	    is_valid = (articles.count(start_article_line) > 0) && (start_article_line != "");
      }

      //SECOND INPUT---------------------------------------------------------
      is_valid = false;
      std::cout << "Enter the exact name of the destination article: ";
      std::string end_article_line;
      getline(std::cin, end_article_line);
      std::cout << std::endl;
      if(end_article_line == "") {
          return 0;
      }
      is_valid = articles.count(end_article_line) > 0;
      while(!is_valid) {
          std::cout << "Invalid article, please enter another title: ";
          getline(std::cin, end_article_line);
          std::cout << std::endl;
          if(end_article_line == "") {
            return 0;
          }
          is_valid = articles.count(start_article_line) > 0;
      }

      //BREADTH OR DEPTH INPUT--------------------------------------------------
      is_valid = false;
      bool search_type_switch; //breadth is false, depth is true
      std::cout << "Enter 0 for breadth search, 1 for depth search, or click ENTER with no input to exit: ";
      std::string search_type_line;
      getline(std::cin, search_type_line);
      std::cout << std::endl;
      while(!is_valid){
        if(search_type_line == ""){
            return 0;
        }
        else if(search_type_line == "0"){
            search_type_switch = 0;
            is_valid = true;
        }
        else if(search_type_line == "1"){
            search_type_switch = 1;
            is_valid = true;
        }
        else{
            std::cout << "Invalid input, please try again: ";
            getline(std::cin, search_type_line);
            std::cout << std::endl;
        }
      }


      std::uint32_t start = articles[start_article_line];
      std::uint32_t end = articles[end_article_line];
      //std::cout << "start: " << std::to_string(start) << "\tend: " << std::to_string(end) << std::endl;

      //FINAL OUTPUT-----------------------------------------------------------
      Timer.unpause();
      std::cout << "Searching for the shortest paths using ";
      if(search_type_switch){ //depth
        std::cout << "depth first search... " << std::endl;
        if(!printPath(searcher.depth_search(start, end))){
            std::cout << "No connections found or degree of seperation less than 2" << std::endl;
        }
      }
      else{ //breadth
        std::cout << "breadth first search... " << std::endl;
        if(!printPath(searcher.breadth_search(end, start))){
            std::cout << "No connections found" << std::endl;
        }
      }
      Timer.printTime();
      Timer.pause();
      std::cout << std::endl;
    }

    //Timer.unpause();
    //printPath(searcher.depth_search(3, 96));
    //std::cout << "breadth" << std::endl;
    //printPath(searcher.reverse_list(searcher.breadth_search(3, 96)));
    //Timer.printTime();
    //Timer.pause();


    return 0;
};
