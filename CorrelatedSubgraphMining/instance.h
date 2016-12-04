/**
 * This is a value of a hastable that its key is DFS_CODE
*/
#ifndef INSTANCE_H
#define INSTANCE_H

#include "graph.h"
#include "graphcode.h"
#include "overlapgraph.h"
#include <vector>
#include <string>

using namespace std;

class Instance
{
public:
	vector<Graph> graphs;
	set<string> childIDs;
	int freq;
	vector<int> independent_graphs;

	Instance()
	{
		this->freq = 0;
	}

	bool isExist(string& dfsCode);
	void push(Graph& g, set<string> child);
	void push(Graph& g);
	int computeFrequency();
};

#endif