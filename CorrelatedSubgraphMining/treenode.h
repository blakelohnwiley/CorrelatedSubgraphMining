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
	int idnode;
	set<string> childIDs;
	string code;
	
	TreeNode()
	{
		isStop = false;
	}

	TreeNode(int id)
	{
		idnode = id;
		isStop = false;
	}

	string computeDFSCode();
	bool isDuplicated(Graph& g);
};

#endif