#pragma once
#include <string>
#include <vector>
#include <set>
#include <unordered_set>
#include <iostream>

struct searchTree{
private:


    //kept global for depth search
    int shortestpath = INT_MAX;

    //kept global for all searches
    std::vector<std::vector<int>> paths;


    //possibily future function that gets data
    //for now it is a placeholder
    std::vector<int> get_children(int parent) {
        std::vector<int> returnVec;
        return returnVec;
    }

public:
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
        depth_helper(path, localvisited, 0, target, start);

        //clean up and return
        std::vector<std::vector<int>> pathscopy = paths;
        shortestpath = INT_MAX;
        paths.clear();
        return pathscopy;
    }
    void depth_helper(std::vector<int>& v, std::unordered_set<int>& vis, int curdepth, int target, int current) {
        //tap out checks
        if (curdepth > shortestpath) { return; }
        if (vis.count(current)) { return; } //if count != 0

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
        vis.insert(current);
        std::vector<int> children = get_children(current);
        for (int child: children) {
            depth_helper(v, vis, curdepth + 1, target, child);
        }
        //wrap up recurse
        vis.erase(current);
        v.pop_back();
    }

    //breadth first search
    std::vector<std::vector<int>> breadth_search(int target){
        std::vector<std::vector<int>> returnVec;
        std::vector<int> curlevel;
        return returnVec;
    }
};