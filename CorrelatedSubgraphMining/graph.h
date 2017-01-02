/*
 *  All graphs in this program have ids of vertices starting from 0
 *	If a graph has n vertices, ids of vertices have to be from 0 to n - 1
 */
#ifndef GRAPH_H
#define GRAPH_H

#include <stdint.h>
#include<iostream>
#include<map>
#include<vector>
#include<algorithm>
#include<fstream>
#include<deque>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <time.h>
#include <cstdlib>

using namespace std;

template <typename T>
inline void _swap(T &x,T &y)
{
	T z = x;
	x = y;
	y = z;
}

class CodeId
{
public:
	uint64_t id1, id2;

	bool operator == (const CodeId & code) const
	{
		return ((id1 == code.id1 && id2 == code.id2) || (id1 == code.id2 && id2 == code.id1));
	}

	bool operator < (const CodeId & code) const
	{
		int min1 = (id1 < id2) ? id1 : id2;
		int min2 = (code.id1 < code.id2) ? code.id1 : code.id2;

		return (min1 < min2);
	}

	bool operator > (const CodeId & code) const
	{
		int max1 = (id1 > id2) ? id1 : id2;
		int max2 = (code.id1 > code.id2) ? code.id1 : code.id2;

		return (max1 > max2);
	}
};

class Edge
{
public:
	int from;
	int to;
	int elabel;
	unsigned int id;
	
	Edge(int _from = -1, int _to = -1, int _elabel = -1, unsigned int _id = -1)
	{
		from = from;
		to = _to;
		elabel = _elabel;
		id = _id;
	}

	bool operator== (const Edge& e) const
	{
		//return (((from == e.from && to == e.to) || (from == e.to && to == e.from)) && elabel == e.elabel);
		return (from == e.from && to == e.to && elabel == e.elabel);
	}

	bool operator != (const Edge& e) const
	{
		return (!(*this == e));
	}
};

class Vertex
{
public:
	typedef vector<Edge>::iterator edge_iterator;
	
	int id;
	int label;
	vector<Edge> edge;
	int depth;
	
	void push(int from, int to, int elabel)
	{
		edge.resize(edge.size()+1);
		edge[edge.size()-1].from = from;
		edge[edge.size()-1].to = to;
		edge[edge.size()-1].elabel = elabel;		
	}

	bool operator< (const Vertex& v) const
	{
		if (id < v.id)
		{
			return true;
		}
		else if (id == v.id)
		{
			return (label < v.label);
		}

		return false;
	}

	bool operator== (const Vertex& v) const
	{
		return (id == v.id && label == v.label);
	}

	bool operator!= (const Vertex& v) const
	{
		return (!((*this) == v));
	}
};

class Graph : public vector<Vertex>
{
private:
	// mapping from id of vertex to index in the stored vector
	map<int, int> mapIdToIndex;
	unsigned int edge_size_;
	bool _isSorted;

public:
	//set <CodeId> sameHHop;
	unordered_set <uint64_t> sameHHop;
	uint64_t idGraph;
	bool directed;

	// hashtable with key: label of vertex, value: a list of index of vertices containing the same lable with key
	unordered_map < int, vector<int> > labelIdx;

	typedef std::vector<Vertex>::iterator vertex_iterator;
	
	bool isSorted() const
	{
		return this->_isSorted;
	}

	unsigned int edge_size() const
	{ 
		return edge_size_;
	}
	unsigned int vertex_size() const
	{ 
		return (unsigned int)size(); 
	}
	
	int index(int id);
	void buildEdge();
	void insertEdge(Graph& g, const Edge& e);
	void insertEdge(Edge& e, int vertexStartLable, int vertexEndLable);
	void extendByVertex(Graph& g, Vertex& v);
	void insertVertex(Vertex& v);
	int vertexLabel(int id);
	// Read data of graph from file
	int read (char* filename);
	ostream& write (ostream & os);
	ofstream& write (ofstream & of);
	void check(void);

	//Check whether two graph are duplicated
	bool isDuplicated(const Graph& g) const;
	//sort all vertices of graph following ascendant order
	void sortGaph();

	// Check if an edge exists or not
	bool isExist(Edge& e);
	bool isExisedVertice(Vertex& v);

	bool overlap (Graph& g);

	Graph() : edge_size_(0){ _isSorted = false; };
	Graph(bool directed) : edge_size_(0){ _isSorted = false; this->directed = directed; };
	Graph(Graph* g);

	//void pushHHopCode(CodeId& id);

	void findNodeinHhop(const int & vertexId, const int & hop, vector<Vertex>& results);

	// get neighbor of a node, return a list of index of neighboring vertices
	vector<int> getNeighbor(int id);
	vector<int> getVerticesByLabel(int label);

	bool operator==(const Graph& g) const;

	vector<Graph> getDownNeighborsExactGraph();
	vector<Graph> getUpNeigborsExactGraph(Graph& database);
	bool isReachable(int idSource, int idDes);

	vector<Graph> getDownNeighborsInducedGraph();
	vector<Graph> getUpNeighborsInducedGraph(Graph& database);
	bool isConnected();
	void convertDigraph2Undirect(unordered_set<int> & visitedId);
	void DFSTraversal(int vId, unordered_set<int> & visitedId);

	Graph initAnyFrequentGraph(int threshold, int & outFreq);
};

#endif