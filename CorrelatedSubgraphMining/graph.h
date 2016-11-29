/*
 *  All graphs in this program have ids of vertices starting from 0
 *	If a graph has n vertices, ids of vertices have to be from 0 to n - 1
 */
#ifndef GRAPH_H
#define GRAPH_H

#include<iostream>
#include<map>
#include<vector>
#include<set>
#include<algorithm>
#include<fstream>

using namespace std;

template <typename T>
inline void _swap(T &x,T &y)
{
	T z = x;
	x = y;
	y = z;
}

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
		return (((from == e.from && to == e.to) || (from == e.to && to == e.from)) && elabel == e.elabel);
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

public:
	
	typedef std::vector<Vertex>::iterator vertex_iterator;
	
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
	int vertexLabel(int id);
	// Read data of graph from file
	int read (char* filename);
	ostream& write (ostream & os);
	ofstream& write (ofstream & of);
	void check(void);

	//Check whether two graph are duplicated
	bool isDuplicated(const Graph& g);
	//sort all vertices of graph following ascendant order
	void sortGaph();

	bool overlap (Graph& g);

	Graph() : edge_size_(0){};
};

#endif