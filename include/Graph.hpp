#include <vector>

class Graph
{

    protected:     // Allows inheritance 
    int n;
    std::vector<std::pair<int,int>> edges;

    public:
        //Constructors
        Graph();
        Graph(int nodes);
        Graph(const Graph &toCopy);
        // Deconstructer for better memory Managment
        ~Graph();

        int numberOfNodes() const;
        bool addEdge(int from,int to);

};


