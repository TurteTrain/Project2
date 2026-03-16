#pragma once
#include <string>
#include <vector>
#include <set>
#include <iostream>

struct searchTree{
private:
    //article node holds info
    struct anode{
        int id;
        std::vector<int> idpaths;

        anode(int _id, std::vector<int> _idpaths): id(_id), idpaths(_idpaths){};
    };

    anode* root;

    //set of all visited nodes
    std::set<int> visited;

public:
    searchTree(int _id, std::vector<int> _idpaths){
        root->id = _id;
        root->idpaths = _idpaths;
    };
    //destructor will depend on how the rest pans out
    //if we make the visited set store anodes rather than ids we could use it to delete them all after
    //but turning every id into an anode may be computationally intensive
    ~searchTree() {}

    //---------------------------------------------------------------------------------------------
    //SEARCHES
    //return a list with all shortest possible paths
    //maybe helper functions for recursion?
    //---------------------------------------------------------------------------------------------

    //depth first search (pre-order)
    std::vector<std::vector<int>> depth_search(int target){
        std::vector<std::vector<int>> returnVec;
        int shortestpath = INT_MAX;
        return returnVec;
    };

    //breadth first search
    std::vector<std::vector<int>> breadth_search(int target){
        std::vector<std::vector<int>> returnVec;
        std::vector<int> curlevel;
        return returnVec;
    };
};