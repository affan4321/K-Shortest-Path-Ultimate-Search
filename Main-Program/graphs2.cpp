#include "graphs2.h"
#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <fstream>
#include <string>
#include <sstream>
#include <unordered_map>
#include <chrono>

using namespace std;

//-----------------------------------------------------------------SELECT FILE---------------------------------------------------------------------------

//File 3

string FileName = "classic-who.csv";
int TOTAL_NODES = 377;

//--------------------------------------------------------------NODE DEFINITIONS-------------------------------------------------------------------------

::Node2::Node2(string name): name(name) {}

::Node2::~Node2() {}

string Node2::getName() const {
    return this->name;
}

void Node2::addEdge(Node2* node, int weight){
    this->edge.push_back(node);
    this->weights.push_back(weight);
}

const vector<Node2*>& Node2::getEdges() const {
    return this->edge;
}

const vector<int>& Node2::getWeights() const {
    return this->weights;
}

//---------------------------------------------------------------GRAPH DEFINITIONS-----------------------------------------------------------------------

::Graph2::Graph2(int totalNodes): totalNodes(totalNodes) {}

::Graph2::~Graph2(){
    this->totalNodes = 0;
    for(auto node: this->nodes){
        delete node;
    }
    // Deallocate memory for AdjacencyMatrix
    for(int i=0; i<this->totalNodes; i++){
        delete[] this->AdjacencyMatrix[i];
    }
    delete[] this->AdjacencyMatrix;
}

int Graph2::getTotalNodes(){ 
    return this->totalNodes;
}

const vector<Node2*>& Graph2::getNodes() const {
    return this->nodes;
}

int Graph2::readFromFile(){
    ifstream myFile("../"+FileName);
    if (!myFile.is_open()) {
        cerr << "Error opening file!" << endl;
        return -1;
    }
    string line;
    
    // Skipping initial line
    getline(myFile, line);
    
    while(getline(myFile, line)){
        //cout << line << endl;
        int weight;
        string node1, node2;

        // Creating stringstream instance to extract meaningful information effectively
        stringstream ss2(line);
        getline(ss2, node1, ',');
        getline(ss2, node2, ',');
        ss2 >> weight;
        
        //cout << "Adding edge between Node " << node1 << " and Node " << node2 << endl;

        // Check if node1 already exists, if not, create and store it
        int index1 = -1;
        for (int i = 0; i < nodes.size(); ++i) {
            if (nodes[i]->getName() == node1) {
                index1 = i;
                break;
            }
        }
        if (index1 == -1) {
            index1 = nodes.size();
            nodes.push_back(new Node2(node1));
        }
        
        // Check if node2 already exists, if not, create and store it
        int index2 = -1;
        for (int i = 0; i < nodes.size(); ++i) {
            if (nodes[i]->getName() == node2) {
                index2 = i;
                break;
            }
        }
        if (index2 == -1) {
            index2 = nodes.size();
            nodes.push_back(new Node2(node2));
        }
        
        // Add edge between nodes
        nodes[index1]->addEdge(nodes[index2], weight);
        nodes[index2]->addEdge(nodes[index1], weight);


        //cout << "Value of node1: " << node1 << ", Value of node2: " << node2 << endl;
    }

    myFile.close();
    return 0;
}

void Graph2::printEdges(){
    int count = 0;
    for(const Node2* node : nodes) {
        cout << "<<Node "<< ++count <<">>: " << node->getName() << " <<edges>>: ";
        for (const Node2* edge : node->getEdges()) {
            cout << edge->getName() << ", ";
        }
        cout << endl << endl;
    }
}

int Graph2::getWeightBetweenNodes(const Node2* node1, const Node2* node2) {
    // Find the index of node2 in the edges of node1
    for(size_t i = 0; i < node1->getEdges().size(); ++i) {
        if(node1->getEdges()[i]->getName() == node2->getName()) {
            // Return the weight associated with the edge
            return node1->getWeights()[i];
        }
    }
    // If edge not found, return a default weight or throw an exception
    return -1; // Adjust this value according to your needs
}



int Graph2::init_AdjacencyMatrix(){
    // Creating space
    cout << "Total Nodes:" << this->totalNodes << endl;
    this->AdjacencyMatrix = new int*[this->totalNodes];
    for(int i = 0; i < this->totalNodes; ++i){
        this->AdjacencyMatrix[i] = new int[this->totalNodes]{0};
    }

    // Iterate over nodes to find indices
    for(int i = 0; i < this->totalNodes; ++i) {
        for(const Node2* edge : this->nodes[i]->getEdges()) {
            // Find index of the edge node
            int index = -1;
            for(int j = 0; j < this->totalNodes; ++j) {
                if(nodes[j]->getName() == edge->getName()) {
                    index = j;
                    break;
                }
            }
            if(index != -1) {
                // Set adjacency matrix value to weight if edge exists
                this->AdjacencyMatrix[i][index] = this->getWeightBetweenNodes(nodes[i], nodes[index]);
            }
        }
    }

    return 0;
}



void Graph2::printAdjacencyMatrix(){
    cout << "AdjacencyMatrix: " << endl;
    for(int r=0; r<this->totalNodes; r++){
        for(int c=0; c<this->totalNodes; c++){
            cout << this->AdjacencyMatrix[r][c] << " ";
        }
        cout << endl;
    }
    cout << endl;
}

// Function to get the index of the node given its name
int nodeIndex(const string& nodeName, const vector<Node2*>& nodes) {
    for (int i = 0; i < nodes.size(); ++i) {
        if (nodes[i]->getName() == nodeName) {
            return i;
        }
    }
    // Return -1 if node name is not found
    return -1;
}

// Function to get the name of the node given its index
string nodeName(int index, const vector<Node2*>& nodes) {
    if (index >= 0 && index < nodes.size()) {
        return nodes[index]->getName();
    }
    // Return an empty string if index is out of range
    return "";
}



int* findKShortestSerial(Graph2* graph, const string& startNodeName, const string& endNodeName, int k) {
    // Retrieve nodes from the graph
    const vector<Node2*>& nodes = graph->getNodes();

    // Find the start and end nodes
    int startIndex = nodeIndex(startNodeName, nodes);
    int endIndex = nodeIndex(endNodeName, nodes);

    if (startIndex == -1 || endIndex == -1) {
        cerr << "Start or end node not found!" << endl;
        return nullptr;
    }

    int n = nodes.size();

    // Initialize a 2D vector to store distances
    vector<vector<int>> distances(n, vector<int>(k, numeric_limits<int>::max()));

    // Priority queue to store nodes based on distance
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    pq.push(make_pair(0, startIndex));
    distances[startIndex][0] = 0;

    while (!pq.empty()) {
        int u = pq.top().second;
        int d = pq.top().first;
        pq.pop();

        if (distances[u][k - 1] < d) {
            continue;
        }

        const vector<Node2*>& edges = nodes[u]->getEdges();
        
        for (const Node2* edge : edges) {
            int v = nodeIndex(edge->getName(), nodes);
            int weight = graph->getWeightBetweenNodes(nodes[u], edge);
            
            if (v == -1) {
                continue; // Skip if the node index is not found
            }
            
            if (d + weight < distances[v][k - 1]) {
                distances[v][k - 1] = d + weight;
                sort(distances[v].begin(), distances[v].end());
                pq.push(make_pair(d + weight, v));
            }
        }
    }

    // Copy the k shortest distances into an array
    int* result = new int[k];
    for (int i = 0; i < k; ++i) {
        result[i] = distances[endIndex][i];
    }

    return result;
}


//----------------------------------------------------------------------MAIN FUNCTION------------------------------------------------------------------


int main() {
    ofstream timeFile("../serialTimes.txt", ios::app);
    if (!timeFile.is_open()) {
        cerr << "Error opening times file!" << endl;
        return -1;
    }
    else{
        // Start measuring time
        auto start = std::chrono::high_resolution_clock::now();

        // Create graph and read data from file
        Graph2* graph = new Graph2(TOTAL_NODES);
        graph->readFromFile();

        // Find k shortest paths
        string startNodeName = "A. H. Millington"; // Example start node
        string endNodeName = "Ace"; // Example end node
        int k = 3; // Number of shortest paths to find

        int* shortestPaths = findKShortestSerial(graph, startNodeName, endNodeName, k);

        // Print the k shortest paths
        cout << "K Shortest Paths from " << startNodeName << " to " << endNodeName << ":" << endl;
        for (int i = 0; i < k; ++i) {
            cout << shortestPaths[i] << " ";
        }
        cout << endl;

        // End measuring time
        auto end = std::chrono::high_resolution_clock::now();

        // Calculate duration
        std::chrono::duration<double> duration = end - start;
        std::cout << "Execution time: " << duration.count() << " seconds" << std::endl;

        // Adding to times File
        timeFile << duration.count() << endl;

        delete[] shortestPaths;
        delete graph;

    }
    timeFile.close();
    return 0;
}
