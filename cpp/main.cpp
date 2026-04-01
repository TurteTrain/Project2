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
    std::vector<std::vector<int>> testingGraph = {
        {13,57,22}, {44,2,78}, {1,39,65}, {88,12,40}, {23,91,11},
        {76,5,34}, {19,72,50}, {60,3,81}, {27,66,14}, {92,8,41},
        {55,0,73}, {4,36,95}, {3,47,68}, {0,29,84}, {8,52,33},
        {63,21,70}, {48,96,25}, {67,10,53}, {71,32,90}, {6,85,44},
        {58,99,37}, {15,74,2}, {0,87,61}, {4,69,80}, {77,31,56},
        {16,62,93}, {35,98,45}, {8,59,17}, {83,64,12}, {13,54,79},
        {84,7,42}, {24,86,60}, {18,49,75}, {14,89,6}, {5,43,97},
        {26,51,20}, {11,94,38}, {20,67,9}, {36,82,55}, {2,73,88},
        {3,9,57}, {9,70,1}, {30,65,92}, {34,58,76}, {1,19,83},
        {26,90,61}, {79,16,52}, {12,68,27}, {16,93,71}, {32,77,28},
        {6,85,38}, {35,80,62}, {14,46,95}, {17,63,74}, {29,91,10},
        {10,38,72}, {24,66,87}, {0,40,78}, {20,43,69}, {27,82,13},
        {7,31,94}, {22,45,86}, {25,51,97}, {15,53,41}, {28,75,89},
        {2,42,98}, {8,56,47}, {17,37,84}, {12,67,23}, {23,58,99},
        {15,41,81}, {18,48,36}, {6,55,79}, {10,39,21}, {32,64,53},
        {5,43,92}, {24,49,88}, {46,72,0}, {57,91,29}, {72,46,33},
        {38,59,7}, {59,38,13}, {28,44,90}, {44,28,70}, {13,30,67},
        {19,50,4}, {31,61,22}, {22,56,76}, {33,64,11}, {18,45,82},
        {45,83,28}, {30,67,0}, {9,42,74}, {36,60,16}, {11,52,86},
        {60,36,16}, {93,25,48}, {34,62,89}, {20,69,48}, {97,65,63},
        {62,97,34}, {20,69,48} };

    searchTree searcher;
    //timer Timer;
    //Timer.startTimer();
    //Timer.pause();
    Parser parser;

    //many path 0->84
    //std::cout << "SHORTEST PATH FROM " << 0 << " TO " << 24 << std::endl;
    //std::cout << "depth" << std::endl;
    //parser.parse_pageid_map();
    //parser.map_ids();

    //parser.make_file_smaller();
    //Timer.unpause();
    std::uint32_t start = parser.pageid_to_linkid(9228);
    std::uint32_t end = parser.pageid_to_linkid(38930);
    printPath(searcher.breadth_search(start, end));
    //printPath(searcher.breadth_search(223834, 9228));
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
