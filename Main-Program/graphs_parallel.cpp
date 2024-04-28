
#include "graphs.h"
#include <fstream>
#include <string>
#include <sstream>
#include <queue>
#include <algorithm>
#include <limits>
#include <stack>
#include <mpi.h>
#include <omp.h>
#include <time.h>
using namespace std;

int TOTAL_NODES;

//-----------------------------------------------------------------SELECT FILE---------------------------------------------------------------------------
//File 1

//string FileName = "Email-EuAll.txt";

//File 2

string FileName = "Email-Enron.txt";

//--------------------------------------------------------------NODE DEFINITIONS-------------------------------------------------------------------------

::Node::Node(int name): name(name) {}

::Node::~Node() {}

int ::Node::getName() const { return this->name; }

void ::Node::addEdge(Node* node){ this->edge.push_back(node); }

const vector<Node*>& ::Node::getEdges() const { return this->edge; }


//---------------------------------------------------------------GRAPH DEFINITIONS-----------------------------------------------------------------------

::Graph::Graph(int totalNodes): totalNodes(totalNodes) {
    nodes.resize(totalNodes, NULL);
    for (int i=0; i<totalNodes; i++){
        this->nodes[i] = new Node(i);
    }
}

::Graph::~Graph(){
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

int ::Graph::getTotalNodes(){ 
    return this->totalNodes;
}

int ::Graph::readFromFile(){
    ifstream myFile("../"+FileName);
    if (!myFile.is_open()) {
        cerr << "Error opening file!" << endl;
        return -1;
    }
    string line;
    // Skipping initial lines
    for (int i = 0; i < 3; ++i) {
        getline(myFile, line);
    }

    int nodes, edges;
    string msg;
    // Creating stringstream instance to extract total nodes and edges
    stringstream ss(line);
    ss >> msg >> msg >> nodes >> msg >> edges;
   // cout << "Total Nodes: " << nodes << ", Total Edges: " << edges << endl;

    // Updating the graph
    this->totalNodes = nodes;

    getline(myFile, line);
    
    while(getline(myFile, line)){
        //cout << line << endl;
        int node1, node2;

        // Creating stringstream instance to extract meaningful information effectively
        stringstream ss2(line);
        ss2 >> node1 >> node2;
        
        //cout << "Adding edge between Node " << node1 << " and Node " << node2 << endl;
        this->nodes[node1]->addEdge(this->nodes[node2]);

        //cout << "Value of node1: " << node1 << ", Value of node2: " << node2 << endl;
    }

    myFile.close();
    return 0;
}

void ::Graph::printEdges(){
    for(const Node* node : nodes) {
        cout << "Node " << node->getName() << " edges: ";
        for (const Node* edge : node->getEdges()) {
            cout << edge->getName() << " ";
        }
        cout << endl;
    }
}

int ::Graph::init_AdjacencyMatrix(){
    // Creating space
    this->AdjacencyMatrix = new int*[this->totalNodes];
    
    for(int i=0; i<this->totalNodes; i++){
        this->AdjacencyMatrix[i] = new int[this->totalNodes]{0};
    }

    for(const Node* node : nodes) {
        for (const Node* edge : node->getEdges()) {
            this->AdjacencyMatrix[node->getName()][edge->getName()] = 1;
        }
    }

    return 0;
}

int** Graph::getAdjacencyList() {
    return this->AdjacencyMatrix;
}

void Graph::deleteAdjacencyList() {
    for (int i = 0; i < totalNodes; ++i) {
        delete[] this->AdjacencyMatrix[i];
    }
    delete[] this->AdjacencyMatrix;
}

vector<Node*>& Graph::getNodes() {
    return nodes;
}

int* findKShortest(Graph* graph, int startNode, int endNode, int k) {
    int n = graph->getTotalNodes();
    vector<Node*>& nodes = graph->getNodes(); // Using getNodes() to access the nodes vector

    vector<vector<int>> dis(n + 1, vector<int>(k, numeric_limits<int>::max()));

    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;
    pq.push({0, startNode});
    dis[startNode][0] = 0;

    while (!pq.empty()) {
        int u = pq.top().second;
        int d = pq.top().first; 
        pq.pop();
        if (dis[u][k - 1] < d)
            continue;

        const vector<Node*>& edges = nodes[u]->getEdges();
        
        #pragma omp for 
        for (const Node* edge : edges) {
            int v = edge->getName();
            int cost = 1; // Assuming all edges have a cost of 1
                 
            if (d + cost < dis[v][k - 1]) {
            #pragma omp critical
            {    
                dis[v][k - 1] = d + cost;
                sort(dis[v].begin(), dis[v].end());
                pq.push({d + cost, v});
            }
            }
        }
    }

    // for (int i = 0; i < k; i++) {
    //     cout << dis[endNode][i] << " ";
    // }
    int * distances = new int [k];
    #pragma omp for 
    for(int i=0;i<k;i++){

        distances[i] = dis[endNode][i]+1 ;
        if(distances[i]<=0){
            #pragma omp critical
            {distances[i] = 2147483647;}
        }
        // cout << dis[endNode][i] << " ";
        // cout<<distances[i]<<" \n";

    }
    return distances;
}

int Graph::findNumberOfNeighbours(int start) {
    for (const Node* node : this->getNodes()) {
        if (node->getName() == start) {
            return node->getEdges().size();
        }
    }
    // Return -1 if the node with the given name is not found
    return -1;
}

const Node* Graph::getNode(int start) {

    for (const Node* node : this->getNodes()) {
        if (node->getName() == start) {
            return node;
        }
    }

    return nullptr;
}
void quicksort(int* arr, int left, int right) {
    if (left < right) {
        int pivot = arr[(left + right) / 2];
        int i = left, j = right;
        while (i <= j) {
            while (arr[i] < pivot) i++;
            while (arr[j] > pivot) j--;
            if (i <= j) {
                swap(arr[i], arr[j]);
                i++;
                j--;
            }
        }
        quicksort(arr, left, j);
        quicksort(arr, i, right);
    }
}


//----------------------------------------------------------------------MAIN FUNCTION-----------------------------------------------------------------------


int main(int argc, char** argv) {
    ofstream timeFile("../parallelTimes.txt", ios::app);
    if (!timeFile.is_open()) {
        cerr << "Error opening times file!" << endl;
        return -1;
    }
    else{
        // Setting up nodes based on file
        if(FileName == "Email-EuAll.txt"){
            TOTAL_NODES = 265214;
        }
        else if(FileName == "Email-Enron.txt"){
            TOTAL_NODES = 36692;
        }

        MPI_Init(&argc, &argv);

        int rank, size, k=3;
        MPI_Comm_rank(MPI_COMM_WORLD, &rank);
        MPI_Comm_size(MPI_COMM_WORLD, &size);
        srand(time(0)); // Seed the random number generator with current time

        int startNodeId = 0;

        srand(time(0)); // Seed the random number generator with current time
        double startTime, endTime, runtime; // Variables to store start time, end time, and runtime

        startTime = MPI_Wtime(); // Record the start time
        int endNodeId = 10;
        
        Graph* graph = new Graph(TOTAL_NODES);
        graph->readFromFile();
        int findNumberOfNeighbours=graph->findNumberOfNeighbours(startNodeId);
        int* recieveShortestDistances = new int [k*findNumberOfNeighbours];

        # pragma omp parallel for
        for(int i = 0; i<k; i++){
            recieveShortestDistances[i]=999999999;
        }

        if (size > findNumberOfNeighbours) {
            if (rank == 0) {
                cout << "More processes than neighbors.\nYou should have "<<findNumberOfNeighbours<<" processes for ideal computation.\nTerminating." << endl;
            }
            MPI_Finalize();
            return 0;
        }

        const Node *startNode = graph->getNode(startNodeId);

        int* kShortestDistance = findKShortest(graph,startNode->edge[rank]->getName(),endNodeId,k);


        MPI_Gather(kShortestDistance,k,MPI_INT, recieveShortestDistances,k,MPI_INT, 0,MPI_COMM_WORLD);
        if(rank==0){
            quicksort(recieveShortestDistances, 0, k * size - 1);

            int * shortestK = new int [k];
            cout<<"\nk shortest paths are from "<<startNodeId<<" to "<<endNodeId<<": ";

            # pragma omp parallel for
            for(int i = 0; i<k;i++){
                shortestK[i]= recieveShortestDistances[i];
                cout<<shortestK[i]<<" ";
            }
            cout<<endl;

            endTime = MPI_Wtime(); // Record the end time
            runtime = endTime - startTime; // Calculate the runtime

            cout << "Runtime on process : " << runtime << " seconds" << endl;

            // Adding to times File
            timeFile << runtime << endl;
        }
    }

    timeFile.close();
    MPI_Finalize();
    return 0;
}