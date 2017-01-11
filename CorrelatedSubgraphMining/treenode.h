#ifndef TREENODE_H
#define TREENODE_H

#include "graph.h"
#include "graphcode.h"
#include "graphtomindfscode.h"
#include <set>
#include <stdint.h>
#include <unordered_set>

using namespace std;

class TreeNode
{
public:
	Graph graph;
	bool isStop;
	uint64_t idnode;
	unordered_set<DFSCode> childIDs;
	DFSCode code;
	unordered_set<CodeId> ignoreList;

	bool isSavedToTable;
	
	TreeNode()
	{
		isStop = false;
		isSavedToTable = false;
	}

	TreeNode(uint64_t id)
	{
		idnode = id;
		isStop = false;
		isSavedToTable = false;
	}

	DFSCode computeDFSCode();
	bool isDuplicated(Graph& g);
};

#endif