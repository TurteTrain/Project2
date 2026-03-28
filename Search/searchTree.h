#pragma once
#include <string>
#include <vector>
#include <set>
#include <unordered_set>
#include <iostream>
#include <climits>

struct searchTree{
private:

    //kept global for depth search
    int shortestpath = INT_MAX;
    std::vector<std::vector<int>> paths;

    //special node that points to parent needed for breadth search
    struct breadthNode {
        int value;
        bool marked = false; //if target
        breadthNode* parent;
        breadthNode(int _value, breadthNode* _parent): value(_value), parent(_parent){}
    };


    //temporary vector to pass in for testing
    std::vector<std::vector<int>> testVec;

    std::vector<int> get_children(int parent) {
        std::vector<int> returnVec = testVec.at(parent);
        return returnVec;
    }

    //PRIVATE HELPER
    void depth_helper(std::vector<int>& v, std::unordered_set<int>& visited, int curdepth, int current, int target) {
        //tap out checks
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
        std::vector<int> children = get_children(current);
        for (int child: children) {
            depth_helper(v, visited, curdepth + 1, child, target);
        }
        //wrap up recurse
        visited.erase(current);
        v.pop_back();
    }

public:
    explicit searchTree(std::vector<std::vector<int>>& testV){
        testVec = testV;
    }
    searchTree()= default;
    ~searchTree() = default;

    //---------------------------------------------------------------------------------------------
    //SEARCHES
    //return a list with all shortest possible paths
    //maybe helper functions for recursion?
    //---------------------------------------------------------------------------------------------

    //depth first search
    std::vector<std::vector<int>> depth_search(int start, int target){
        //containers to be reused
        std::vector<int> path;
        std::unordered_set<int> localvisited;

        //first function call
        depth_helper(path, localvisited, 0, start, target);

        //clean up and return
        std::vector<std::vector<int>> pathscopy = paths;
        shortestpath = INT_MAX;
        paths.clear();
        return pathscopy;
    }

    //breadth first search
    std::vector<std::vector<int>> breadth_search(int start, int target){
        std::vector<std::vector<int>> localpaths;
        std::unordered_set<int> localvisited = {start};
        std::vector<int> toSearch = {start};
        std::vector<int> nextLevel;
        bool search = true;

        breadthNode* root = new breadthNode(start, nullptr);
        breadthNode* currentNode = root;
        std::vector<breadthNode*> parentNodes = {root};
        int i = 0;

        //for deletion
        std::vector<breadthNode*> deletionQueue = {root};

        while (search) {
            std::vector<breadthNode*> childrenNodes;
            for (int current: toSearch) {
                    std::vector<int> children = get_children(current);
                    currentNode = parentNodes.at(i);
                    i++;
                    for (int child: children) {
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
            if (toSearch.size() == 0) {
                search = false;
            }
            parentNodes = childrenNodes;
        }
        //parent nodes will now contain all nodes of level where target was found
        //we now have to build each path
        for (breadthNode* node: parentNodes) {
            if (node->marked == true) {
                std::vector<int> tempPath;
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

    std::vector<std::vector<int>> reverse_list(const std::vector<std::vector<int>>& v) {
        std::vector<std::vector<int>> retVec;
        for (std::vector<int> path: v) {
            std::vector<int> tempPath;
            for (int i = path.size() - 1; i >= 0; i--) {
                tempPath.push_back(path.at(i));
            }
            retVec.push_back(tempPath);
        }
        return retVec;
    }
};