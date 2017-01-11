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
#include <unordered_set>
#include <stdint.h>

using namespace std;

class Instance
{
public:
	vector<Graph> graphs;
	unordered_set<DFSCode> childIDs;
	int freq;
	//vector<int> independent_graphs;
	unordered_set<CodeId> ignoreList;
	unordered_map<uint64_t, unsigned int> mapIdToIndexGraph;

	Instance()
	{
		this->freq = 0;
	}

	const Instance & operator = (const Instance & ins);

	bool isExist(DFSCode& dfsCode);
	void push(Graph& g, unordered_set<DFSCode> child);
	void push(Graph& g, unordered_set<DFSCode> child, unordered_set<CodeId> ignore);
	void push(Graph& g);
	void assign(vector<Graph>& listGraph);
	void assign(vector<Graph>& listGraph, int freq_);
	void assign(vector<Graph>& listGraph, unordered_set<CodeId> ignore);
	void insertIgnoreList(unordered_set<CodeId>& ignore);
	int computeFrequency();
};

#endif