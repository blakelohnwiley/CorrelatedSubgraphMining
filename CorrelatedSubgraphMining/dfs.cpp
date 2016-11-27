#include "graphcode.h"
#include <cstring>
#include <string>
#include <iterator>
#include <set>
using namespace std;

bool DFSCode::toGraph(Graph& g)
{
	g.clear();

	for (DFSCode::iterator it = begin(); it != end(); ++it) 
	{
		g.resize(max(it->from, it->to) + 1);

		if (it->fromlabel != -1)
		{
			g[it->from].id = it->from;
			g[it->from].label = it->fromlabel;
		}

		if (it->tolabel != -1)
		{
			g[it->to].id = it->to;
			g[it->to].label = it->tolabel;
		}

		g[it->from].push(it->from, it->to, it->elabel);
		g[it->to].push(it->to, it->from, it->elabel);
	}

	g.buildEdge();

	return true;
}

unsigned int DFSCode::nodeCount()
{
	unsigned int nodecount = 0;

	for(DFSCode::iterator it = begin(); it != end(); ++it)
		nodecount = max(nodecount, (unsigned int) (max(it->from,it->to) + 1));
	
	return nodecount;
}

ostream& DFSCode::write(ostream& os)
{
	if(size() == 0) return os;
	
	for(unsigned int i=0; i < (unsigned int) size(); ++i)
	{
		os << "(" << (*this)[i].from << " " << (*this)[i].to << " " << (*this)[i].fromlabel << " " << (*this)[i].elabel << " " << (*this)[i].tolabel << ")";
	}

	os << endl;
	return os;
}