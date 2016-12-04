#ifndef GRAPHCODE_H
#define GRAPHCODE_H

#include "graph.h"

#include<iostream>
#include<map>
#include<vector>
#include<fstream>
#include <string>

using namespace std;

class DFS
{
public:
	int from;
	int to;
	int fromlabel;
	int elabel;
	int tolabel;

	bool operator == (DFS& d2) const
	{
		return (from == d2.from && to == d2.to && fromlabel == d2.fromlabel && elabel == d2.elabel && tolabel == d2.tolabel);
	}
	
	bool operator != (DFS& d2) const
	{
		return (!(*this == d2));
	}
	DFS() : from(-1), to(-1), fromlabel(-1), elabel(-1), tolabel(-1) {};
};

typedef vector<int> RMPath;

class DFSCode : public vector<DFS>
{
private:
	RMPath rmpath;

public:
	const RMPath& buildRMPath();
	
	unsigned int nodeCount(void);

	void push(int from, int to, int fromlabel, int elabel, int tolabel)
	{
		resize(size()+1);
		DFS& d = (*this)[size()-1];
		
		d.from = from;
		d.to = to;
		d.fromlabel = fromlabel;
		d.elabel = elabel;
		d.tolabel = tolabel;
	}
	
	void pop()
	{ 
		resize (size() - 1);
	}
	ostream& write(ostream &);

	/* Convert current DFS code into a graph.
	 */
	bool toGraph(Graph &);
	string toString();
};

struct PDFS
{
	unsigned int id;
	Edge *edge;
	PDFS *prev;
	PDFS() : id(0), edge(0), prev(0) {};
};

class History : public vector<Edge*>
{
private:
	vector<int> edge;
	vector<int> vertex;

public:
	bool hasEdge(unsigned int id)
	{
		return (bool)edge[id];
	}
	
	bool hasVertex(unsigned int id)
	{
		return (bool)vertex[id];
	}

	void build(Graph& g, PDFS* pdfs);

	History() {};
	History(Graph& g, PDFS* p)
	{
		build(g, p);
	}
};

class Projected : public vector<PDFS>
{
public:
	void push(int id, Edge* edge, PDFS* prev)
	{
		resize(size()+1);
		PDFS& d = (*this)[size()-1];
		d.id = id;
		d.edge = edge;
		d.prev = prev;
	}
};

typedef vector<Edge*> EdgeList;
typedef map<int, map<int, map<int,Projected> > >					Projected_map3;
typedef map<int, map<int, Projected> >								Projected_map2;
typedef map<int, Projected>											Projected_map1;
typedef map<int, map<int, map<int,Projected> > >::iterator			Projected_iterator3;
typedef map<int, map<int, Projected> >::iterator						Projected_iterator2;
typedef map<int, Projected>::iterator								Projected_iterator1;
typedef map<int, map<int, map<int, Projected> > >::reverse_iterator	Projected_riterator3;

bool get_forward_pure(Graph& g, Edge* e, int i, History& h, EdgeList& el);
bool get_forward_rmpath(Graph& g, Edge* e, int i, History& h, EdgeList& el);
bool get_forward_root(Graph& g, Vertex& v, EdgeList& el);
Edge* get_backward(Graph& g, Edge* e1, Edge* e2, History& h);


#endif