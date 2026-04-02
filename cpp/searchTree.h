#pragma once
#include <string>
#include <vector>
#include <set>
#include <unordered_set>
#include <iostream>
#include <climits>
//#include "../cpp/link_data.hpp"
#include "parser.h"

struct searchTree{
private:
    //kept global for depth search
  std::uint32_t shortestpath = INT_MAX;
  std::vector<std::vector<std::uint32_t>> paths;

    //special node that points to parent needed for breadth search
    struct breadthNode {
        int value;
        bool marked = false; //if target
        breadthNode* parent;
        breadthNode(int _value, breadthNode* _parent): value(_value), parent(_parent){}
    };

  //GraphParents graph_parent;
  /*
    std::vector<int> get_parents(int id) {
        std::vector<int> returnVec;
        auto parents = graph_parent[id];
        for (auto parent: parents) {
            returnVec.push_back(static_cast<uint32_t>(parent));
        }

        return returnVec;
    }
  */

  Parser parser;

    //PRIVATE HELPER
  void depth_helper(std::vector<std::uint32_t>& v, std::unordered_set<std::uint32_t>& visited, int curdepth, std::uint32_t current, std::uint32_t target) {
        //tap out checks
        if(curdepth > 2){ return; }
        if (curdepth > shortestpath) { return; }
        if (visited.count(current)) { return; } //if count != 0

        //node visit is allowed
        v.push_back(current);

        //if found target
        if (current == target) {
            //new shortest path
            if (curdepth < shortestpath) {
                shortestpath = curdepth;
                paths.clear();
            }
            paths.push_back(v);
            v.pop_back(); //remove target from list and keep searching
            return;
        }

        //recurse
        visited.insert(current);
        std::vector<std::uint32_t> children = parser.get_parents(parser.pageid_to_linkid(current));
        for (std::uint32_t child: children) {
            depth_helper(v, visited, curdepth + 1, child, target);
        }
        //wrap up recurse
        visited.erase(current);
        v.pop_back();
    }


public:
  //explicit searchTree() : graph_parent("db/links.bin", "db/link_offsets.bin") {}

    //searchTree()= default;
    ~searchTree() = default;

    //---------------------------------------------------------------------------------------------
    //SEARCHES
    //return a list with all shortest possible paths
    //maybe helper functions for recursion?
    //---------------------------------------------------------------------------------------------

    //depth first search
  std::vector<std::vector<std::uint32_t>> depth_search(std::uint32_t start, std::uint32_t target){
        //containers to be reused
    std::vector<std::uint32_t> path;
    std::unordered_set<std::uint32_t> localvisited;

        //first function call
        depth_helper(path, localvisited, 0, start, target);

        //clean up and return
        std::vector<std::vector<std::uint32_t>> pathscopy = paths;
        shortestpath = INT_MAX;
        paths.clear();
        return pathscopy;
    }

    //breadth first search
  std::vector<std::vector<std::uint32_t>> breadth_search(std::uint32_t start, std::uint32_t target){
    std::vector<std::vector<std::uint32_t>> localpaths;
    std::unordered_set<std::uint32_t> localvisited = {start};
    std::vector<std::uint32_t> toSearch = {start};
    std::vector<std::uint32_t> nextLevel;
        bool search = true;

        breadthNode* root = new breadthNode(start, nullptr);
        breadthNode* currentNode = root;
        std::vector<breadthNode*> parentNodes = {root};
        int i = 0;

        //for deletion
        std::vector<breadthNode*> deletionQueue = {root};

        while (search) {
            std::vector<breadthNode*> childrenNodes;
            for (std::uint32_t current: toSearch) {
	      std::vector<std::uint32_t> children = parser.get_parents(parser.pageid_to_linkid(current));
              currentNode = parentNodes.at(i);
              i++;
              for (std::uint32_t child: children) {
		breadthNode* childNode = new breadthNode(child, currentNode);
		if (child == target) {
		  childNode->marked = true;
		  childrenNodes.push_back(childNode);
		  nextLevel.push_back(child);
		  deletionQueue.push_back(childNode);
		  search = false;
		}
		else if (!localvisited.count(child)) {
		  childrenNodes.push_back(childNode);
		  nextLevel.push_back(child);
		  localvisited.insert(child);
		  deletionQueue.push_back(childNode);
		}
		else {
		  delete childNode;
		}
	      }
            }
            toSearch = nextLevel;
            nextLevel.clear();
            i = 0;
            if (toSearch.empty()) {
	      search = false;
            }
            parentNodes = childrenNodes;
        }
        //parent nodes will now contain all nodes of level where target was found
        //we now have to build each path
        for (breadthNode* node: parentNodes) {
            if (node->marked == true) {
	      std::vector<std::uint32_t> tempPath;
                while (node) { //while node isn't nullptr
                    tempPath.push_back(node->value);
                    node = node->parent;
                }
                localpaths.push_back(tempPath);
            }
        }

        //deletion
        for (breadthNode* node: deletionQueue) {
            delete node;
        }
        //------------

        return localpaths;
    }

  std::vector<std::vector<std::uint32_t>> reverse_list(const std::vector<std::vector<std::uint32_t>>& v) {
    std::vector<std::vector<std::uint32_t>> retVec;
        for (std::vector<std::uint32_t> path: v) {
	  std::vector<std::uint32_t> tempPath;
            for (int i = path.size() - 1; i >= 0; i--) {
                tempPath.push_back(path.at(i));
            }
            retVec.push_back(tempPath);
        }
        return retVec;
    }
};
