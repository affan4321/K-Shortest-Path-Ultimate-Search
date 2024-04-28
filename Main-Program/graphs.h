#pragma once
#include <iostream>
#include <vector>
using namespace std;

/*
    PDC Project by Muhammad Affan (21i-0474), Faraz Rashid (21i-0659), Ahmad Hassam (21i-0403)
    Project Name: "K-Shortest Path Ultimate Search"
    Copyright Â© 2024 Muhammad Affan, Faraz Rashid, Ahmad Hassan

    Documentation:
    We have a Graph Object which have multiple nodes. This is stored as totalNodes.
    each Node can have multiple edges to other nodes. So each Node has 2d pointer to other nodes.
*/

#ifndef GRAPH_H
#define GRAPH_H

class Node{
private:
    int name;
   
public:
    vector<Node*> edge;

    // Constructor
    Node(int name = -1);

    // member functions
    int getName() const;
    void addEdge(Node* node);
    const vector<Node*>& getEdges() const;

    //Destructor
    ~Node();
};

struct SparseMatrixEntry {
    int row;
    int col;
    int value;
};

class Graph{
private:
    int totalNodes;
    vector<Node*> nodes;
    int** AdjacencyMatrix;
    vector<SparseMatrixEntry> sparseMatrixEntries;

public:
    // Constructor
    Graph(int totalNodes = 0);

    // member Functions
    int getTotalNodes();
    int readFromFile();
    int init_AdjacencyMatrix();
    void printEdges();
    void printAdjacencyMatrix();
    int** getAdjacencyList();
    void deleteAdjacencyList();
    vector<Node*>& getNodes(); 
    int findNumberOfNeighbours(int start);
    const Node * getNode(int start);

    // Destructor
    ~Graph();
};

#endif