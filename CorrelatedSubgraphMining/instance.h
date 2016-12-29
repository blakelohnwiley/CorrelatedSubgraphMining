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
#include <set>
#include <unordered_map>

using namespace std;

class Instance
{
public:
	vector<Graph> graphs;
	set<DFSCode> childIDs;
	int freq;
	//vector<int> independent_graphs;
	set<CodeId> ignoreList;
	unordered_map<double, int> mapIdToIndexGraph;

	Instance()
	{
		this->freq = 0;
	}

	bool isExist(DFSCode& dfsCode);
	void push(Graph& g, set<DFSCode> child);
	void push(Graph& g, set<DFSCode> child, set<CodeId> ignore);
	void push(Graph& g);
	int computeFrequency();
};

#endif