#ifndef GRAPHCODE_H
#define GRAPHCODE_H

#include "graph.h"

#include<iostream>
#include<map>
#include<vector>
#include<fstream>
#include <string>
#include <functional>
#include <unordered_map>

using namespace std;

class DFS
{
public:
	int from;
	int to;
	int fromlabel;
	int elabel;
	int tolabel;

	bool operator == (const DFS& d2) const
	{
		return (from == d2.from && to == d2.to && fromlabel == d2.fromlabel && elabel == d2.elabel && tolabel == d2.tolabel);
	}
	
	bool operator != (const DFS& d2) const
	{
		return (!(*this == d2));
	}

	bool operator < (const DFS& d2) const
	{
		if (from < d2.from)
			return true;
		else if (from == d2.from)
		{
			if (to < d2.to)
				return true;
			else if (to == d2.to)
			{
				if (fromlabel < d2.fromlabel)
					return true;
				else if (fromlabel == d2.fromlabel)
				{
					if (elabel < d2.elabel)
						return true;
					else if (elabel == d2.elabel)
					{
						return tolabel < d2.tolabel;
					}
					else
						return false;
				}
				else
					return false;
			}
			else 
				return false;
		}
		else 
			return false;
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
	
	bool operator < (const DFSCode & d2) const
	{
		if (this->size() < d2.size())
			return true;
		else if (this->size() == d2.size())
		{
			for (unsigned int i = 0; i < (unsigned int)this->size(); i++)
			{
				if ((*this)[i] < d2[i])
				{
					return true;
				}
				else if ((*this)[i] == d2[i])
					continue;
				else
					return false;
			}

			return false;
		}
		else return false;
	}

	bool operator == (const DFSCode & d2) const
	{
		unsigned int tmpSize = this->size();

		if (tmpSize != d2.size())
			return false;

		for (unsigned int i = 0; i < tmpSize; i++)
		{
			if ((*this)[i] != d2[i])
				return false;
		}

		return true;
	}

	bool operator != (const DFSCode & d2) const
	{
		return (!(*this == d2));
	}

	void pop()
	{ 
		resize (size() - 1);
	}
	ostream& write(ostream &);

	/* Convert current DFS code into a graph.
	 */
	bool toGraph(Graph &);
};

struct PDFS
{
	unsigned int id;
	Edge *edge;
	PDFS *prev;
	PDFS() : id(0), edge(0), prev(0) {};
};

namespace std
{
	template <class T>
	void hash_combine(std::size_t& seed, T v)
	{
		seed ^= std::hash<T>() (v) + 0x9e3779b9 + (seed<<6) + (seed>>2);
	}

    template<> struct hash<DFS>
    {
		std::size_t operator()(const DFS & s) const
        {
			std::size_t hash = 0;

			hash_combine(hash, s.from);
			hash_combine(hash, s.to);
			hash_combine(hash, s.fromlabel);
			hash_combine(hash, s.elabel);
			hash_combine(hash, s.tolabel);
            
			return hash;
        }
    };

	template<> struct hash<DFSCode>
    {
        std::size_t operator()(const DFSCode & s) const
        {
			std::size_t seed = s.size();
            
			for (DFS i : s)
			{
				hash_combine(seed, hash<DFS>() (i));
			}

			return seed;
        }
    };
}

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