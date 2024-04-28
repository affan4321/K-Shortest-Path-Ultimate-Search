#pragma once
#include <iostream>
#include <vector>
using namespace std;

/*
    PDC Project by Muhammad Affan (21i-0474), Faraz Rashid (21i-0659), Ahmad Hassam (21i-0403)
    Project Name: "K-Shortest Path Ultimate Search"
    Copyright © 2024 Muhammad Affan, Faraz Rashid, Ahmad Hassan

    Documentation:
    We have a Graph Object which have multiple nodes. This is stored as totalNodes.
    each Node can have multiple edges to other nodes. So each Node has 2d pointer to other nodes.

    ** This file deals with all the string related datasets and files **
*/

#ifndef GRAPH2_H
#define GRAPH2_H


class Node2{
private:
    string name;
    vector<Node2*> edge;
    vector<int> weights;

public:
    // Constructor
    Node2(string name = "");

    // member functions
    string getName() const;
    void addEdge(Node2* node, int weight);
    const vector<Node2*>& getEdges() const;
    const vector<int>& getWeights() const;


    //Destructor
    ~Node2();
};


class Graph2{
private:
    vector<int> indexes;
    int totalNodes;
    vector<Node2*> nodes;
    int** AdjacencyMatrix;

public:
    // Constructor
    Graph2(int totalNodes = 0);

    // member Functions
    
    int getTotalNodes();
    int readFromFile();
    void printEdges();
    int init_AdjacencyMatrix();
    void printAdjacencyMatrix();
    const vector<Node2*>& getNodes() const;
    int getWeightBetweenNodes(const Node2* node1, const Node2* node2); 
    int nodeIndex(const string& nodeName, const vector<Node2*>& nodes);
    string nodeName(int index, const vector<Node2*>& nodes);

    // Destructor
    ~Graph2();
};

#endif