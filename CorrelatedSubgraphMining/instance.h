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
#include <stdint.h>

using namespace std;

class Instance
{
public:
	vector<Graph> graphs;
	set<DFSCode> childIDs;
	int freq;
	//vector<int> independent_graphs;
	set<CodeId> ignoreList;
	unordered_map<uint64_t, int> mapIdToIndexGraph;

	Instance()
	{
		this->freq = 0;
	}

	bool isExist(DFSCode& dfsCode);
	void push(Graph& g, set<DFSCode> child);
	void push(Graph& g, set<DFSCode> child, set<CodeId> ignore);
	void push(Graph& g);
	void assign(vector<Graph>& listGraph);
	void assign(vector<Graph>& listGraph, int freq_);
	void assign(vector<Graph>& listGraph, set<CodeId> ignore);
	void insertIgnoreList(set<CodeId>& ignore);
	int computeFrequency();
};

#endif