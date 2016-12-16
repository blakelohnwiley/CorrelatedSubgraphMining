#ifndef TREENODE_H
#define TREENODE_H

#include "graph.h"
#include "graphcode.h"
#include "graphtomindfscode.h"
#include <set>

using namespace std;

class TreeNode
{
public:
	Graph graph;
	bool isStop;
	double idnode;
	set<DFSCode> childIDs;
	DFSCode code;
	set<CodeId> ignoreList;

	bool isSavedToTable;
	
	TreeNode()
	{
		isStop = false;
		isSavedToTable = false;
	}

	TreeNode(double id)
	{
		idnode = id;
		isStop = false;
		isSavedToTable = false;
	}

	DFSCode computeDFSCode();
	bool isDuplicated(Graph& g);
};

#endif